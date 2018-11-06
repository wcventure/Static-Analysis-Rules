int ff_jpeg2000_init_component(Jpeg2000Component *comp,
                               Jpeg2000CodingStyle *codsty,
                               Jpeg2000QuantStyle *qntsty,
                               int cbps, int dx, int dy,
                               AVCodecContext *avctx)
{
    uint8_t log2_band_prec_width, log2_band_prec_height;
    int reslevelno, bandno, gbandno = 0, ret, i, j;
    uint32_t csize;

    if (codsty->nreslevels2decode <= 0) {
        av_log(avctx, AV_LOG_ERROR, "nreslevels2decode %d invalid or uninitialized\n", codsty->nreslevels2decode);
        return AVERROR_INVALIDDATA;
    }

    if (ret = ff_jpeg2000_dwt_init(&comp->dwt, comp->coord,
                                   codsty->nreslevels2decode - 1,
                                   codsty->transform))
        return ret;
    // component size comp->coord is uint16_t so ir cannot overflow
    csize = (comp->coord[0][1] - comp->coord[0][0]) *
            (comp->coord[1][1] - comp->coord[1][0]);

    if (codsty->transform == FF_DWT97) {
        csize += FF_INPUT_BUFFER_PADDING_SIZE / sizeof(*comp->f_data);
        comp->i_data = NULL;
        comp->f_data = av_mallocz_array(csize, sizeof(*comp->f_data));
        if (!comp->f_data)
            return AVERROR(ENOMEM);
    } else {
        csize += FF_INPUT_BUFFER_PADDING_SIZE / sizeof(*comp->i_data);
        comp->f_data = NULL;
        comp->i_data = av_mallocz_array(csize, sizeof(*comp->i_data));
        if (!comp->i_data)
            return AVERROR(ENOMEM);
    }
    comp->reslevel = av_mallocz_array(codsty->nreslevels, sizeof(*comp->reslevel));
    if (!comp->reslevel)
        return AVERROR(ENOMEM);
    /
    for (reslevelno = 0; reslevelno < codsty->nreslevels; reslevelno++) {
        int declvl = codsty->nreslevels - reslevelno;    // N_L -r see  ISO/IEC 15444-1:2002 B.5
        Jpeg2000ResLevel *reslevel = comp->reslevel + reslevelno;

        /
        for (i = 0; i < 2; i++)
            for (j = 0; j < 2; j++)
                reslevel->coord[i][j] =
                    ff_jpeg2000_ceildivpow2(comp->coord_o[i][j], declvl - 1);
        // update precincts size: 2^n value
        reslevel->log2_prec_width  = codsty->log2_prec_widths[reslevelno];
        reslevel->log2_prec_height = codsty->log2_prec_heights[reslevelno];

        /
        if (reslevelno == 0)
            reslevel->nbands = 1;
        else
            reslevel->nbands = 3;

        /
        if (reslevel->coord[0][1] == reslevel->coord[0][0])
            reslevel->num_precincts_x = 0;
        else
            reslevel->num_precincts_x =
                ff_jpeg2000_ceildivpow2(reslevel->coord[0][1],
                                        reslevel->log2_prec_width) -
                (reslevel->coord[0][0] >> reslevel->log2_prec_width);

        if (reslevel->coord[1][1] == reslevel->coord[1][0])
            reslevel->num_precincts_y = 0;
        else
            reslevel->num_precincts_y =
                ff_jpeg2000_ceildivpow2(reslevel->coord[1][1],
                                        reslevel->log2_prec_height) -
                (reslevel->coord[1][0] >> reslevel->log2_prec_height);

        reslevel->band = av_mallocz_array(reslevel->nbands, sizeof(*reslevel->band));
        if (!reslevel->band)
            return AVERROR(ENOMEM);

        for (bandno = 0; bandno < reslevel->nbands; bandno++, gbandno++) {
            Jpeg2000Band *band = reslevel->band + bandno;
            int cblkno, precno;
            int nb_precincts;

            /
            switch (qntsty->quantsty) {
                uint8_t gain;
            case JPEG2000_QSTY_NONE:
                /
                band->f_stepsize = 1;
                break;
            case JPEG2000_QSTY_SI:
                /
//                 numbps = cbps +
//                          lut_gain[codsty->transform == FF_DWT53][bandno + (reslevelno > 0)];
//                 band->f_stepsize = SHL(2048 + qntsty->mant[gbandno],
//                                        2 + numbps - qntsty->expn[gbandno]);
//                 break;
            case JPEG2000_QSTY_SE:
                /
                /
                gain            = cbps;
                band->f_stepsize  = pow(2.0, gain - qntsty->expn[gbandno]);
                band->f_stepsize *= qntsty->mant[gbandno] / 2048.0 + 1.0;
                break;
            default:
                band->f_stepsize = 0;
                av_log(avctx, AV_LOG_ERROR, "Unknown quantization format\n");
                break;
            }
            /
            if (!av_codec_is_encoder(avctx->codec))
                band->f_stepsize *= 0.5;

            band->i_stepsize = band->f_stepsize * (1 << 15);

            /
            if (reslevelno == 0) {
                /
                for (i = 0; i < 2; i++)
                    for (j = 0; j < 2; j++)
                        band->coord[i][j] =
                            ff_jpeg2000_ceildivpow2(comp->coord_o[i][j],
                                                    declvl - 1);
                log2_band_prec_width  = reslevel->log2_prec_width;
                log2_band_prec_height = reslevel->log2_prec_height;
                /
                band->log2_cblk_width  = FFMIN(codsty->log2_cblk_width,
                                               reslevel->log2_prec_width);
                band->log2_cblk_height = FFMIN(codsty->log2_cblk_height,
                                               reslevel->log2_prec_height);
            } else {
                /
                /
                for (i = 0; i < 2; i++)
                    for (j = 0; j < 2; j++)
                        /
                        band->coord[i][j] =
                            ff_jpeg2000_ceildivpow2(comp->coord_o[i][j] -
                                                    (((bandno + 1 >> i) & 1) << declvl - 1),
                                                    declvl);
                /

                /
                band->log2_cblk_width  = FFMIN(codsty->log2_cblk_width,
                                               reslevel->log2_prec_width - 1);
                band->log2_cblk_height = FFMIN(codsty->log2_cblk_height,
                                               reslevel->log2_prec_height - 1);

                log2_band_prec_width  = reslevel->log2_prec_width  - 1;
                log2_band_prec_height = reslevel->log2_prec_height - 1;
            }

            if (reslevel->num_precincts_x * (uint64_t)reslevel->num_precincts_y > INT_MAX) {
                band->prec = NULL;
                return AVERROR(ENOMEM);
            }
            nb_precincts = reslevel->num_precincts_x * reslevel->num_precincts_y;
            band->prec = av_mallocz_array(nb_precincts, sizeof(*band->prec));
            if (!band->prec)
                return AVERROR(ENOMEM);

            for (precno = 0; precno < nb_precincts; precno++) {
                Jpeg2000Prec *prec = band->prec + precno;
                int nb_codeblocks;

                /
                /

                /
                prec->coord[0][0] = ((band->coord[0][0] >> reslevel->log2_prec_width) + precno % reslevel->num_precincts_x) *
                                    (1 << log2_band_prec_width);
                prec->coord[0][0] = FFMAX(prec->coord[0][0], band->coord[0][0]);

                /
                prec->coord[1][0] = ((band->coord[1][0] >> reslevel->log2_prec_height) + precno / reslevel->num_precincts_x) *
                                    (1 << log2_band_prec_height);
                prec->coord[1][0] = FFMAX(prec->coord[1][0], band->coord[1][0]);

                /
                prec->coord[0][1] = prec->coord[0][0] +
                                    (1 << log2_band_prec_width);
                prec->coord[0][1] = FFMIN(prec->coord[0][1], band->coord[0][1]);

                /
                prec->coord[1][1] = prec->coord[1][0] +
                                    (1 << log2_band_prec_height);
                prec->coord[1][1] = FFMIN(prec->coord[1][1], band->coord[1][1]);

                prec->nb_codeblocks_width =
                    ff_jpeg2000_ceildivpow2(prec->coord[0][1],
                                            band->log2_cblk_width)
                    - (prec->coord[0][0] >> band->log2_cblk_width);
                prec->nb_codeblocks_height =
                    ff_jpeg2000_ceildivpow2(prec->coord[1][1],
                                            band->log2_cblk_height)
                    - (prec->coord[1][0] >> band->log2_cblk_height);


                /
                prec->cblkincl =
                    ff_jpeg2000_tag_tree_init(prec->nb_codeblocks_width,
                                              prec->nb_codeblocks_height);
                if (!prec->cblkincl)
                    return AVERROR(ENOMEM);

                prec->zerobits =
                    ff_jpeg2000_tag_tree_init(prec->nb_codeblocks_width,
                                              prec->nb_codeblocks_height);
                if (!prec->zerobits)
                    return AVERROR(ENOMEM);

                if (prec->nb_codeblocks_width * (uint64_t)prec->nb_codeblocks_height > INT_MAX) {
                    prec->cblk = NULL;
                    return AVERROR(ENOMEM);
                }
                nb_codeblocks = prec->nb_codeblocks_width * prec->nb_codeblocks_height;
                prec->cblk = av_mallocz_array(nb_codeblocks, sizeof(*prec->cblk));
                if (!prec->cblk)
                    return AVERROR(ENOMEM);
                for (cblkno = 0; cblkno < nb_codeblocks; cblkno++) {
                    Jpeg2000Cblk *cblk = prec->cblk + cblkno;
                    uint16_t Cx0, Cy0;

                    /
                    /
                    Cx0 = ((prec->coord[0][0]) >> band->log2_cblk_width) << band->log2_cblk_width;
                    Cx0 = Cx0 + ((cblkno % prec->nb_codeblocks_width)  << band->log2_cblk_width);
                    cblk->coord[0][0] = FFMAX(Cx0, prec->coord[0][0]);

                    /
                    Cy0 = ((prec->coord[1][0]) >> band->log2_cblk_height) << band->log2_cblk_height;
                    Cy0 = Cy0 + ((cblkno / prec->nb_codeblocks_width)   << band->log2_cblk_height);
                    cblk->coord[1][0] = FFMAX(Cy0, prec->coord[1][0]);

                    /
                    cblk->coord[0][1] = FFMIN(Cx0 + (1 << band->log2_cblk_width),
                                              prec->coord[0][1]);

                    /
                    cblk->coord[1][1] = FFMIN(Cy0 + (1 << band->log2_cblk_height),
                                              prec->coord[1][1]);
                    /
                    if ((bandno + !!reslevelno) & 1) {
                        cblk->coord[0][0] += comp->reslevel[reslevelno-1].coord[0][1] -
                                             comp->reslevel[reslevelno-1].coord[0][0];
                        cblk->coord[0][1] += comp->reslevel[reslevelno-1].coord[0][1] -
                                             comp->reslevel[reslevelno-1].coord[0][0];
                    }
                    if ((bandno + !!reslevelno) & 2) {
                        cblk->coord[1][0] += comp->reslevel[reslevelno-1].coord[1][1] -
                                             comp->reslevel[reslevelno-1].coord[1][0];
                        cblk->coord[1][1] += comp->reslevel[reslevelno-1].coord[1][1] -
                                             comp->reslevel[reslevelno-1].coord[1][0];
                    }

                    cblk->zero      = 0;
                    cblk->lblock    = 3;
                    cblk->length    = 0;
                    memset(cblk->lengthinc, 0, sizeof(cblk->lengthinc));
                    cblk->npasses   = 0;
                }
            }
        }
    }
    return 0;
}
