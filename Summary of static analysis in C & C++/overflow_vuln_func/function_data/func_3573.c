static void dwt_decode97_float(DWTContext *s, float *t)
{
    int lev;
    int w       = s->linelen[s->ndeclevels - 1][0];
    float *line = s->f_linebuf;
    float *data = t;
    /
    line += 5;

    for (lev = 0; lev < s->ndeclevels; lev++) {
        int lh = s->linelen[lev][0],
            lv = s->linelen[lev][1],
            mh = s->mod[lev][0],
            mv = s->mod[lev][1],
            lp;
        float *l;
        // HOR_SD
        l = line + mh;
        for (lp = 0; lp < lv; lp++) {
            int i, j = 0;
            // copy with interleaving
            for (i = mh; i < lh; i += 2, j++)
                l[i] = data[w * lp + j] * F_LFTG_K;
            for (i = 1 - mh; i < lh; i += 2, j++)
                l[i] = data[w * lp + j];

            sr_1d97_float(line, mh, mh + lh);

            for (i = 0; i < lh; i++)
                data[w * lp + i] = l[i];
        }

        // VER_SD
        l = line + mv;
        for (lp = 0; lp < lh; lp++) {
            int i, j = 0;
            // copy with interleaving
            for (i = mv; i < lv; i += 2, j++)
                l[i] = data[w * j + lp] * F_LFTG_K;
            for (i = 1 - mv; i < lv; i += 2, j++)
                l[i] = data[w * j + lp];

            sr_1d97_float(line, mv, mv + lv);

            for (i = 0; i < lv; i++)
                data[w * i + lp] = l[i];
        }
    }
}
