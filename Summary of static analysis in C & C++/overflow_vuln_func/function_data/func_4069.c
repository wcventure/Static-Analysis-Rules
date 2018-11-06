static void filter1(int32_t *dst, const int32_t *src, int32_t coeff, ptrdiff_t len)
{
    int i;

    for (i = 0; i < len; i++)
        dst[i] -= mul23(src[i], coeff);
}
