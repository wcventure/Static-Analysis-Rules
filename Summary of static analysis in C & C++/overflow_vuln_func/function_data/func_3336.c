static int eval_refl(int *refl, const int16_t *coefs, RA144Context *ractx)
{
    int b, i, j;
    int buffer1[10];
    int buffer2[10];
    int *bp1 = buffer1;
    int *bp2 = buffer2;

    for (i=0; i < 10; i++)
        buffer2[i] = coefs[i];

    refl[9] = bp2[9];

    if ((unsigned) bp2[9] + 0x1000 > 0x1fff) {
        av_log(ractx, AV_LOG_ERROR, "Overflow. Broken sample?\n");
        return 1;
    }

    for (i=8; i >= 0; i--) {
        b = 0x1000-((bp2[i+1] * bp2[i+1]) >> 12);

        if (!b)
            b = -2;

        for (j=0; j <= i; j++)
            bp1[j] = ((bp2[j] - ((refl[i+1] * bp2[i-j]) >> 12)) * (0x1000000 / b)) >> 12;

        refl[i] = bp1[i];

        if ((unsigned) bp1[i] + 0x1000 > 0x1fff)
            return 1;

        FFSWAP(int *, bp1, bp2);
    }
    return 0;
}
