static int make_ydt24_entry(int p1, int p2, int16_t *ydt)
{
    int lo, hi;

    lo = ydt[p1];
    hi = ydt[p2];
    return (lo + (hi << 8) + (hi << 16)) << 1;
}
