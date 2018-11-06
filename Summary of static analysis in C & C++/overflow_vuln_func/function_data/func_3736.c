static void decor_c(int32_t *dst, const int32_t *src, int coeff, ptrdiff_t len)
{
    int i;

    for (i = 0; i < len; i++)
        dst[i] += src[i] * coeff + (1 << 2) >> 3;
}
