int ff_j2k_init_component(Jpeg2000Component *comp, Jpeg2000CodingStyle *codsty, Jpeg2000QuantStyle *qntsty, int cbps, int dx, int dy)
{
    int reslevelno, bandno, gbandno = 0, ret, i, j, csize = 1;

    if (ret=ff_j2k_dwt_init(&comp->dwt, comp->coord, codsty->nreslevels-1, codsty->transform))
        return ret;
    for (i = 0; i < 2; i++)
        csize *= comp->coord[i][1] - comp->coord[i][0];

    comp->data = av_malloc(csize * sizeof(int));
    if (!comp->data)
        return AVERROR(ENOMEM);
    comp->reslevel = av_malloc(codsty->nreslevels * sizeof(Jpeg2000ResLevel));

    if (!comp->reslevel)
        return AVERROR(ENOMEM);
    for (reslevelno = 0; reslevelno < codsty->nreslevels; reslevelno++) {
        int declvl = codsty->nreslevels - reslevelno;
        Jpeg2000ResLevel *reslevel = comp->reslevel + reslevelno;

        for (i = 0; i < 2; i++)
            for (j = 0; j < 2; j++)
                reslevel->coord[i][j] =
                    ff_jpeg2000_ceildivpow2(comp->coord[i][j], declvl - 1);

        if (reslevelno == 0)
            reslevel->nbands = 1;
        else
            reslevel->nbands = 3;

        if (reslevel->coord[0][1] == reslevel->coord[0][0])
            reslevel->num_precincts_x = 0;
        else
            reslevel->num_precincts_x = ff_jpeg2000_ceildivpow2(reslevel->coord[0][1], codsty->log2_prec_width)
                                        - (reslevel->coord[0][0] >> codsty->log2_prec_width);

        if (reslevel->coord[1][1] == reslevel->coord[1][0])
            reslevel->num_precincts_y = 0;
        else
            reslevel->num_precincts_y = ff_jpeg2000_ceildivpow2(reslevel->coord[1][1], codsty->log2_prec_height)
                                        - (reslevel->coord[1][0] >> codsty->log2_prec_height);

        reslevel->band = av_malloc(reslevel->nbands * sizeof(Jpeg2000Band));
        if (!reslevel->band)
            return AVERROR(ENOMEM);
        for (bandno = 0; bandno < reslevel->nbands; bandno++, gbandno++) {
            Jpeg2000Band *band = reslevel->band + bandno;
            int cblkno, precx, precy, precno;
            int x0, y0, x1, y1;
            int xi0, yi0, xi1, yi1;
            int cblkperprecw, cblkperprech;

            if (qntsty->quantsty != JPEG2000_QSTY_NONE) {
                static const uint8_t lut_gain[2][4] = {{0, 0, 0, 0}, {0, 1, 1, 2}};
                int numbps;

                numbps = cbps + lut_gain[codsty->transform][bandno + reslevelno>0];
                band->stepsize = SHL(2048 + qntsty->mant[gbandno], 2 + numbps - qntsty->expn[gbandno]);
            } else
                band->stepsize = 1 << 13;

            if (reslevelno == 0) {  // the same everywhere
                band->codeblock_width = 1 << FFMIN(codsty->log2_cblk_width, codsty->log2_prec_width-1);
                band->codeblock_height = 1 << FFMIN(codsty->log2_cblk_height, codsty->log2_prec_height-1);
                for (i = 0; i < 2; i++)
                    for (j = 0; j < 2; j++)
                        band->coord[i][j] = ff_jpeg2000_ceildivpow2(comp->coord[i][j], declvl-1);
            } else{
                band->codeblock_width = 1 << FFMIN(codsty->log2_cblk_width, codsty->log2_prec_width);
                band->codeblock_height = 1 << FFMIN(codsty->log2_cblk_height, codsty->log2_prec_height);

                for (i = 0; i < 2; i++)
                    for (j = 0; j < 2; j++)
                        band->coord[i][j] = ff_jpeg2000_ceildivpow2(comp->coord[i][j] - (((bandno+1>>i)&1) << declvl-1), declvl);
            }
            band->cblknx = ff_jpeg2000_ceildiv(band->coord[0][1], band->codeblock_width)  - band->coord[0][0] / band->codeblock_width;
            band->cblkny = ff_jpeg2000_ceildiv(band->coord[1][1], band->codeblock_height) - band->coord[1][0] / band->codeblock_height;

            for (j = 0; j < 2; j++)
                band->coord[0][j] = ff_jpeg2000_ceildiv(band->coord[0][j], dx);
            for (j = 0; j < 2; j++)
                band->coord[1][j] = ff_jpeg2000_ceildiv(band->coord[1][j], dy);

            band->cblknx = ff_jpeg2000_ceildiv(band->cblknx, dx);
            band->cblkny = ff_jpeg2000_ceildiv(band->cblkny, dy);

            band->cblk = av_malloc(sizeof(Jpeg2000Cblk) * band->cblknx * band->cblkny);
            if (!band->cblk)
                return AVERROR(ENOMEM);
            band->prec = av_malloc(sizeof(Jpeg2000Cblk) * reslevel->num_precincts_x * reslevel->num_precincts_y);
            if (!band->prec)
                return AVERROR(ENOMEM);

            for (cblkno = 0; cblkno < band->cblknx * band->cblkny; cblkno++) {
                Jpeg2000Cblk *cblk = band->cblk + cblkno;
                cblk->zero = 0;
                cblk->lblock = 3;
                cblk->length = 0;
                cblk->lengthinc = 0;
                cblk->npasses = 0;
            }

            y0 = band->coord[1][0];
            y1 = ((band->coord[1][0] + (1<<codsty->log2_prec_height)) & ~((1<<codsty->log2_prec_height)-1)) - y0;
            yi0 = 0;
            yi1 = ff_jpeg2000_ceildivpow2(y1 - y0, codsty->log2_cblk_height) << codsty->log2_cblk_height;
            yi1 = FFMIN(yi1, band->cblkny);
            cblkperprech = 1<<(codsty->log2_prec_height - codsty->log2_cblk_height);
            for (precy = 0, precno = 0; precy < reslevel->num_precincts_y; precy++) {
                for (precx = 0; precx < reslevel->num_precincts_x; precx++, precno++) {
                    band->prec[precno].yi0 = yi0;
                    band->prec[precno].yi1 = yi1;
                }
                yi1 += cblkperprech;
                yi0 = yi1 - cblkperprech;
                yi1 = FFMIN(yi1, band->cblkny);
            }
            x0 = band->coord[0][0];
            x1 = ((band->coord[0][0] + (1<<codsty->log2_prec_width)) & ~((1<<codsty->log2_prec_width)-1)) - x0;
            xi0 = 0;
            xi1 = ff_jpeg2000_ceildivpow2(x1 - x0, codsty->log2_cblk_width) << codsty->log2_cblk_width;
            xi1 = FFMIN(xi1, band->cblknx);

            cblkperprecw = 1<<(codsty->log2_prec_width - codsty->log2_cblk_width);
            for (precx = 0, precno = 0; precx < reslevel->num_precincts_x; precx++) {
                for (precy = 0; precy < reslevel->num_precincts_y; precy++, precno = 0) {
                    Jpeg2000Prec *prec = band->prec + precno;
                    prec->xi0 = xi0;
                    prec->xi1 = xi1;
                    prec->cblkincl = ff_j2k_tag_tree_init(prec->xi1 - prec->xi0,
                                                          prec->yi1 - prec->yi0);
                    prec->zerobits = ff_j2k_tag_tree_init(prec->xi1 - prec->xi0,
                                                          prec->yi1 - prec->yi0);
                    if (!prec->cblkincl || !prec->zerobits)
                        return AVERROR(ENOMEM);

                }
                xi1 += cblkperprecw;
                xi0 = xi1 - cblkperprecw;
                xi1 = FFMIN(xi1, band->cblknx);
            }
        }
    }
    return 0;
}
