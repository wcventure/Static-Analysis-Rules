static void extrapolate_isf(float isf[LP_ORDER_16k])
{
    float diff_isf[LP_ORDER - 2], diff_mean;
    float *diff_hi = diff_isf - LP_ORDER + 1; // diff array for extrapolated indexes
    float corr_lag[3];
    float est, scale;
    int i, i_max_corr;

    isf[LP_ORDER_16k - 1] = isf[LP_ORDER - 1];

    /
    for (i = 0; i < LP_ORDER - 2; i++)
        diff_isf[i] = isf[i + 1] - isf[i];

    diff_mean = 0.0;
    for (i = 2; i < LP_ORDER - 2; i++)
        diff_mean += diff_isf[i] * (1.0f / (LP_ORDER - 4));

    /
    i_max_corr = 0;
    for (i = 0; i < 3; i++) {
        corr_lag[i] = auto_correlation(diff_isf, diff_mean, i + 2);

        if (corr_lag[i] > corr_lag[i_max_corr])
            i_max_corr = i;
    }
    i_max_corr++;

    for (i = LP_ORDER - 1; i < LP_ORDER_16k - 1; i++)
        isf[i] = isf[i - 1] + isf[i - 1 - i_max_corr]
                            - isf[i - 2 - i_max_corr];

    /
    est   = 7965 + (isf[2] - isf[3] - isf[4]) / 6.0;
    scale = 0.5 * (FFMIN(est, 7600) - isf[LP_ORDER - 2]) /
            (isf[LP_ORDER_16k - 2] - isf[LP_ORDER - 2]);

    for (i = LP_ORDER - 1; i < LP_ORDER_16k - 1; i++)
        diff_hi[i] = scale * (isf[i] - isf[i - 1]);

    /
    for (i = LP_ORDER; i < LP_ORDER_16k - 1; i++)
        if (diff_hi[i] + diff_hi[i - 1] < 5.0) {
            if (diff_hi[i] > diff_hi[i - 1]) {
                diff_hi[i - 1] = 5.0 - diff_hi[i];
            } else
                diff_hi[i] = 5.0 - diff_hi[i - 1];
        }

    for (i = LP_ORDER - 1; i < LP_ORDER_16k - 1; i++)
        isf[i] = isf[i - 1] + diff_hi[i] * (1.0f / (1 << 15));

    /
    for (i = 0; i < LP_ORDER_16k - 1; i++)
        isf[i] *= 0.8;
}
