static void ps_add_squares_c(INTFLOAT *dst, const INTFLOAT (*src)[2], int n)
{
    int i;
    for (i = 0; i < n; i++)
        dst[i] += AAC_MADD28(src[i][0], src[i][0], src[i][1], src[i][1]);
}
