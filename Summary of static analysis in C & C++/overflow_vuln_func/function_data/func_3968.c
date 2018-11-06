int ff_eval_refl(int *refl, const int16_t *coefs, AVCodecContext *avctx)
{
    int b, i, j;
    int buffer1[LPC_ORDER];
    int buffer2[LPC_ORDER];
    int *bp1 = buffer1;
    int *bp2 = buffer2;

    for (i=0; i < LPC_ORDER; i++)
        buffer2[i] = coefs[i];

    refl[LPC_ORDER-1] = bp2[LPC_ORDER-1];

    if ((unsigned) bp2[LPC_ORDER-1] + 0x1000 > 0x1fff) {
        av_log(avctx, AV_LOG_ERROR, "Overflow. Broken sample?\n");
        return 1;
    }

    for (i = LPC_ORDER-2; i >= 0; i--) {
        b = 0x1000-((bp2[i+1] * bp2[i+1]) >> 12);

        if (!b)
            b = -2;

        b = 0x1000000 / b;
        for (j=0; j <= i; j++) {
#if CONFIG_FTRAPV
            int a = bp2[j] - ((refl[i+1] * bp2[i-j]) >> 12);
            if((int)(a*(unsigned)b) != a*(int64_t)b)
                return 1;
#endif
            bp1[j] = ((bp2[j] - ((refl[i+1] * bp2[i-j]) >> 12)) * b) >> 12;
        }

        if ((unsigned) bp1[i] + 0x1000 > 0x1fff)
            return 1;

        refl[i] = bp1[i];

        FFSWAP(int *, bp1, bp2);
    }
    return 0;
}
