int ff_j2k_dwt_init(DWTContext *s, uint16_t border[2][2], int decomp_levels, int type)
{
    int i, j, lev = decomp_levels, maxlen,
        b[2][2];

    if (decomp_levels >= FF_DWT_MAX_DECLVLS)
        return AVERROR_INVALIDDATA;
    s->ndeclevels = decomp_levels;
    s->type = type;

    for (i = 0; i < 2; i++)
        for(j = 0; j < 2; j++)
            b[i][j] = border[i][j];

    maxlen = FFMAX(b[0][1] - b[0][0],
                   b[1][1] - b[1][0]);

    while(--lev >= 0){
        for (i = 0; i < 2; i++){
            s->linelen[lev][i] = b[i][1] - b[i][0];
            s->mod[lev][i] = b[i][0] & 1;
            for (j = 0; j < 2; j++)
                b[i][j] = (b[i][j] + 1) >> 1;
        }
    }
    if (type == FF_DWT97)
        s->linebuf = av_malloc((maxlen + 12) * sizeof(float));
    else if (type == FF_DWT53)
        s->linebuf = av_malloc((maxlen + 6) * sizeof(int));
    else
        return -1;

    if (!s->linebuf)
        return AVERROR(ENOMEM);

    return 0;
}
