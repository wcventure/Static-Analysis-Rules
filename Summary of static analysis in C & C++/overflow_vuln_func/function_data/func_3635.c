static void lpc_analyze_remodulate(int32_t *decoded, const int coeffs[32],
                                   int order, int qlevel, int len, int bps)
{
    int i, j;
    int ebps = 1 << (bps-1);
    unsigned sigma = 0;

    for (i = order; i < len; i++)
        sigma |= decoded[i] + ebps;

    if (sigma < 2*ebps)
        return;

    for (i = len - 1; i >= order; i--) {
        int64_t p = 0;
        for (j = 0; j < order; j++)
            p += coeffs[j] * (int64_t)decoded[i-order+j];
        decoded[i] -= p >> qlevel;
    }
    for (i = order; i < len; i++, decoded++) {
        int32_t p = 0;
        for (j = 0; j < order; j++)
            p += coeffs[j] * (uint32_t)decoded[j];
        decoded[j] += p >> qlevel;
    }
}
