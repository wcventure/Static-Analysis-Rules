static void idiv64(uint64_t *plow, uint64_t *phigh, int64_t b)
{
    int sa, sb;
    sa = ((int64_t)*phigh < 0);
    if (sa)
        neg128(plow, phigh);
    sb = (b < 0);
    if (sb)
        b = -b;
    div64(plow, phigh, b);
    if (sa ^ sb)
        *plow = - *plow;
    if (sa)
        *phigh = - *phigh;
}
