static void truncpasses(Jpeg2000EncoderContext *s, Jpeg2000Tile *tile)
{
    int precno, compno, reslevelno, bandno, cblkno, lev;
    Jpeg2000CodingStyle *codsty = &s->codsty;

    for (compno = 0; compno < s->ncomponents; compno++){
        Jpeg2000Component *comp = tile->comp + compno;

        for (reslevelno = 0, lev = codsty->nreslevels-1; reslevelno < codsty->nreslevels; reslevelno++, lev--){
            Jpeg2000ResLevel *reslevel = comp->reslevel + reslevelno;

            for (precno = 0; precno < reslevel->num_precincts_x * reslevel->num_precincts_y; precno++){
                for (bandno = 0; bandno < reslevel->nbands ; bandno++){
                    int bandpos = bandno + (reslevelno > 0);
                    Jpeg2000Band *band = reslevel->band + bandno;
                    Jpeg2000Prec *prec = band->prec + precno;

                    for (cblkno = 0; cblkno < prec->nb_codeblocks_height * prec->nb_codeblocks_width; cblkno++){
                        Jpeg2000Cblk *cblk = prec->cblk + cblkno;

                        cblk->ninclpasses = getcut(cblk, s->lambda,
                                (int64_t)dwt_norms[codsty->transform == FF_DWT53][bandpos][lev] * (int64_t)band->i_stepsize >> 16);
                    }
                }
            }
        }
    }
}
