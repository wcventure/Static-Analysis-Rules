#if HAVE_BIGENDIAN
static int make_ydt15_entry(int p2, int p1, int16_t *ydt)
#else
static int make_ydt15_entry(int p1, int p2, int16_t *ydt)
#endif
{
    int lo, hi;

    lo = ydt[p1];
    lo += (lo * 32) + (lo * 1024);
    hi = ydt[p2];
    hi += (hi * 32) + (hi * 1024);
    return (lo + (hi * (1 << 16))) * 2;
}
