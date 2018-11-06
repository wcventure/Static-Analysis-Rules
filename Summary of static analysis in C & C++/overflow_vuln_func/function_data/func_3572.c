static void dwt_encode97_float(DWTContext *s, float *t)
{
    int lev,
        w = s->linelen[s->ndeclevels-1][0];
    float *line = s->f_linebuf;
    line += 5;

    for (lev = s->ndeclevels-1; lev >= 0; lev--){
        int lh = s->linelen[lev][0],
            lv = s->linelen[lev][1],
            mh = s->mod[lev][0],
            mv = s->mod[lev][1],
            lp;
        float *l;

        // HOR_SD
        l = line + mh;
        for (lp = 0; lp < lv; lp++){
            int i, j = 0;

            for (i = 0; i < lh; i++)
                l[i] = t[w*lp + i];

            sd_1d97_float(line, mh, mh + lh);

            // copy back and deinterleave
            for (i =   mh; i < lh; i+=2, j++)
                t[w*lp + j] = F_LFTG_X * l[i];
            for (i = 1-mh; i < lh; i+=2, j++)
                t[w*lp + j] = l[i];
        }

        // VER_SD
        l = line + mv;
        for (lp = 0; lp < lh; lp++) {
            int i, j = 0;

            for (i = 0; i < lv; i++)
                l[i] = t[w*i + lp];

            sd_1d97_float(line, mv, mv + lv);

            // copy back and deinterleave
            for (i =   mv; i < lv; i+=2, j++)
                t[w*j + lp] = F_LFTG_X * l[i];
            for (i = 1-mv; i < lv; i+=2, j++)
                t[w*j + lp] = l[i];
        }
    }
}
