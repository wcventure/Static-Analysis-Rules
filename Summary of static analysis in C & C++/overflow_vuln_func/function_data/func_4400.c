static int encode_tile(Jpeg2000EncoderContext *s, Jpeg2000Tile *tile, int tileno)
{
    int compno, reslevelno, bandno, ret;
    Jpeg2000T1Context t1;
    Jpeg2000CodingStyle *codsty = &s->codsty;
    for (compno = 0; compno < s->ncomponents; compno++){
        Jpeg2000Component *comp = s->tile[tileno].comp + compno;

        av_log(s->avctx, AV_LOG_DEBUG,"dwt\n");
        if (ret = ff_dwt_encode(&comp->dwt, comp->i_data))
            return ret;
        av_log(s->avctx, AV_LOG_DEBUG,"after dwt -> tier1\n");

        for (reslevelno = 0; reslevelno < codsty->nreslevels; reslevelno++){
            Jpeg2000ResLevel *reslevel = comp->reslevel + reslevelno;

            for (bandno = 0; bandno < reslevel->nbands ; bandno++){
                Jpeg2000Band *band = reslevel->band + bandno;
                Jpeg2000Prec *prec = band->prec; // we support only 1 precinct per band ATM in the encoder
                int cblkx, cblky, cblkno=0, xx0, x0, xx1, y0, yy0, yy1, bandpos;
                yy0 = bandno == 0 ? 0 : comp->reslevel[reslevelno-1].coord[1][1] - comp->reslevel[reslevelno-1].coord[1][0];
                y0 = yy0;
                yy1 = FFMIN(ff_jpeg2000_ceildivpow2(band->coord[1][0] + 1, band->log2_cblk_height) << band->log2_cblk_height,
                            band->coord[1][1]) - band->coord[1][0] + yy0;

                if (band->coord[0][0] == band->coord[0][1] || band->coord[1][0] == band->coord[1][1])
                    continue;

                bandpos = bandno + (reslevelno > 0);

                for (cblky = 0; cblky < prec->nb_codeblocks_height; cblky++){
                    if (reslevelno == 0 || bandno == 1)
                        xx0 = 0;
                    else
                        xx0 = comp->reslevel[reslevelno-1].coord[0][1] - comp->reslevel[reslevelno-1].coord[0][0];
                    x0 = xx0;
                    xx1 = FFMIN(ff_jpeg2000_ceildivpow2(band->coord[0][0] + 1, band->log2_cblk_width) << band->log2_cblk_width,
                                band->coord[0][1]) - band->coord[0][0] + xx0;

                    for (cblkx = 0; cblkx < prec->nb_codeblocks_width; cblkx++, cblkno++){
                        int y, x;
                        if (codsty->transform == FF_DWT53){
                            for (y = yy0; y < yy1; y++){
                                int *ptr = t1.data[y-yy0];
                                for (x = xx0; x < xx1; x++){
                                    *ptr++ = comp->i_data[(comp->coord[0][1] - comp->coord[0][0]) * y + x] << NMSEDEC_FRACBITS;
                                }
                            }
                        } else{
                            for (y = yy0; y < yy1; y++){
                                int *ptr = t1.data[y-yy0];
                                for (x = xx0; x < xx1; x++){
                                    *ptr = (comp->i_data[(comp->coord[0][1] - comp->coord[0][0]) * y + x]);
                                    *ptr = (int64_t)*ptr * (int64_t)(16384 * 65536 / band->i_stepsize) >> 14 - NMSEDEC_FRACBITS;
                                    ptr++;
                                }
                            }
                        }
                        encode_cblk(s, &t1, prec->cblk + cblkno, tile, xx1 - xx0, yy1 - yy0,
                                    bandpos, codsty->nreslevels - reslevelno - 1);
                        xx0 = xx1;
                        xx1 = FFMIN(xx1 + (1 << band->log2_cblk_width), band->coord[0][1] - band->coord[0][0] + x0);
                    }
                    yy0 = yy1;
                    yy1 = FFMIN(yy1 + (1 << band->log2_cblk_height), band->coord[1][1] - band->coord[1][0] + y0);
                }
            }
        }
        av_log(s->avctx, AV_LOG_DEBUG, "after tier1\n");
    }

    av_log(s->avctx, AV_LOG_DEBUG, "rate control\n");
    truncpasses(s, tile);
    if (ret = encode_packets(s, tile, tileno))
        return ret;
    av_log(s->avctx, AV_LOG_DEBUG, "after rate control\n");
    return 0;
}
