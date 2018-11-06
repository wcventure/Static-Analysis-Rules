static int eval_refl(const int16_t *coefs, int *refl, RA144Context *ractx)
{
    int retval = 0;
    int b, c, i;
    unsigned int u;
    int buffer1[10];
    int buffer2[10];
    int *bp1 = buffer1;
    int *bp2 = buffer2;

    for (i=0; i < 10; i++)
        buffer2[i] = coefs[i];

    u = refl[9] = bp2[9];

    if (u + 0x1000 > 0x1fff) {
        av_log(ractx, AV_LOG_ERROR, "Overflow. Broken sample?\n");
        return 0;
    }

    for (c=8; c >= 0; c--) {
        if (u == 0x1000)
            u++;

        if (u == 0xfffff000)
            u--;

        b = 0x1000-((u * u) >> 12);

        if (b == 0)
            b++;

        for (u=0; u<=c; u++)
            bp1[u] = ((bp2[u] - ((refl[c+1] * bp2[c-u]) >> 12)) * (0x1000000 / b)) >> 12;

        refl[c] = u = bp1[c];

        if ((u + 0x1000) > 0x1fff)
            retval = 1;

        FFSWAP(int *, bp1, bp2);
    }
    return retval;
}
