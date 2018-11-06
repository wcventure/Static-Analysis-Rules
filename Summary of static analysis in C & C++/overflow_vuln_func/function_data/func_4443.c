static void lsp2lpc(int16_t *lpc)
{
    int f1[LPC_ORDER / 2 + 1];
    int f2[LPC_ORDER / 2 + 1];
    int i, j;

    /
    for (j = 0; j < LPC_ORDER; j++) {
        int index     = (lpc[j] >> 7) & 0x1FF;
        int offset    = lpc[j] & 0x7f;
        int temp1     = cos_tab[index] * (1 << 16);
        int temp2     = (cos_tab[index + 1] - cos_tab[index]) *
                          (((offset << 8) + 0x80) << 1);

        lpc[j] = -(av_sat_dadd32(1 << 15, temp1 + temp2) >> 16);
    }

    /
    /
    f1[0] = 1 << 28;
    f1[1] = (lpc[0] + lpc[2]) * (1 << 14);
    f1[2] = lpc[0] * lpc[2] + (2 << 28);

    f2[0] = 1 << 28;
    f2[1] = (lpc[1] + lpc[3]) * (1 << 14);
    f2[2] = lpc[1] * lpc[3] + (2 << 28);

    /
    for (i = 2; i < LPC_ORDER / 2; i++) {
        f1[i + 1] = f1[i - 1] + MULL2(f1[i], lpc[2 * i]);
        f2[i + 1] = f2[i - 1] + MULL2(f2[i], lpc[2 * i + 1]);

        for (j = i; j >= 2; j--) {
            f1[j] = MULL2(f1[j - 1], lpc[2 * i]) +
                    (f1[j] >> 1) + (f1[j - 2] >> 1);
            f2[j] = MULL2(f2[j - 1], lpc[2 * i + 1]) +
                    (f2[j] >> 1) + (f2[j - 2] >> 1);
        }

        f1[0] >>= 1;
        f2[0] >>= 1;
        f1[1] = ((lpc[2 * i]     * 65536 >> i) + f1[1]) >> 1;
        f2[1] = ((lpc[2 * i + 1] * 65536 >> i) + f2[1]) >> 1;
    }

    /
    for (i = 0; i < LPC_ORDER / 2; i++) {
        int64_t ff1 = f1[i + 1] + f1[i];
        int64_t ff2 = f2[i + 1] - f2[i];

        lpc[i] = av_clipl_int32(((ff1 + ff2) * 8) + (1 << 15)) >> 16;
        lpc[LPC_ORDER - i - 1] = av_clipl_int32(((ff1 - ff2) * 8) +
                                                (1 << 15)) >> 16;
    }
}
