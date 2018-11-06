static int rice_count_exact(int32_t *res, int n, int k)
{
    int i;
    int count = 0;

    for (i = 0; i < n; i++) {
        int32_t v = -2 * res[i] - 1;
        v ^= v >> 31;
        count += (v >> k) + 1 + k;
    }
    return count;
}
