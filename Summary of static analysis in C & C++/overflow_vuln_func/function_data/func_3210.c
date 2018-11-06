static int find_optimal_param(uint32_t sum, int n)
{
    int k;
    uint32_t sum2;

    if (sum <= n >> 1)
        return 0;
    sum2 = sum - (n >> 1);
    k    = av_log2(n < 256 ? FASTDIV(sum2, n) : sum2 / n);
    return FFMIN(k, MAX_RICE_PARAM);
}
