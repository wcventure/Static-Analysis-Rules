static int jpeg2000_decode_packet(Jpeg2000DecoderContext *s,
                                  Jpeg2000CodingStyle *codsty,
                                  Jpeg2000ResLevel *rlevel, int precno,
                                  int layno, uint8_t *expn, int numgbits)
{
    int bandno, cblkno, ret, nb_code_blocks;

    if (!(ret = get_bits(s, 1))) {
        jpeg2000_flush(s);
        return 0;
    } else if (ret < 0)
        return ret;

    for (bandno = 0; bandno < rlevel->nbands; bandno++) {
        Jpeg2000Band *band = rlevel->band + bandno;
        Jpeg2000Prec *prec = band->prec + precno;

        if (band->coord[0][0] == band->coord[0][1] ||
            band->coord[1][0] == band->coord[1][1])
            continue;
        prec->yi0 = 0;
        prec->xi0 = 0;
        nb_code_blocks =  prec->nb_codeblocks_height *
                          prec->nb_codeblocks_width;
        for (cblkno = 0; cblkno < nb_code_blocks; cblkno++) {
            Jpeg2000Cblk *cblk = prec->cblk + cblkno;
            int incl, newpasses, llen;

            if (cblk->npasses)
                incl = get_bits(s, 1);
            else
                incl = tag_tree_decode(s, prec->cblkincl + cblkno, layno + 1) == layno;
            if (!incl)
                continue;
            else if (incl < 0)
                return incl;

            if (!cblk->npasses)
                cblk->nonzerobits = expn[bandno] + numgbits - 1 -
                                    tag_tree_decode(s, prec->zerobits + cblkno,
                                                    100);
            if ((newpasses = getnpasses(s)) < 0)
                return newpasses;
            if ((llen = getlblockinc(s)) < 0)
                return llen;
            cblk->lblock += llen;
            if ((ret = get_bits(s, av_log2(newpasses) + cblk->lblock)) < 0)
                return ret;
            if (ret > sizeof(cblk->data)) {
                avpriv_request_sample(s->avctx,
                                      "Block with lengthinc greater than %zu",
                                      sizeof(cblk->data));
                return AVERROR_PATCHWELCOME;
            }
            cblk->lengthinc = ret;
            cblk->npasses  += newpasses;
        }
    }
    jpeg2000_flush(s);

    if (codsty->csty & JPEG2000_CSTY_EPH) {
        if (bytestream2_peek_be16(&s->g) == JPEG2000_EPH)
            bytestream2_skip(&s->g, 2);
        else
            av_log(s->avctx, AV_LOG_ERROR, "EPH marker not found.\n");
    }

    for (bandno = 0; bandno < rlevel->nbands; bandno++) {
        Jpeg2000Band *band = rlevel->band + bandno;
        Jpeg2000Prec *prec = band->prec + precno;

        nb_code_blocks = prec->nb_codeblocks_height * prec->nb_codeblocks_width;
        for (cblkno = 0; cblkno < nb_code_blocks; cblkno++) {
            Jpeg2000Cblk *cblk = prec->cblk + cblkno;
            if (bytestream2_get_bytes_left(&s->g) < cblk->lengthinc)
                return AVERROR_INVALIDDATA;
            /
            if (cblk->lengthinc > 0) {
                bytestream2_get_bufferu(&s->g, cblk->data, cblk->lengthinc);
            } else {
                cblk->data[0] = 0xFF;
                cblk->data[1] = 0xFF;
            }
            cblk->length   += cblk->lengthinc;
            cblk->lengthinc = 0;
        }
    }
    return 0;
}
