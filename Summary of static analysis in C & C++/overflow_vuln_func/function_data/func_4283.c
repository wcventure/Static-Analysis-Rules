static int make_cdt24_entry(int p1, int p2, int16_t *cdt)
{
    int r, b;

    b = cdt[p2];
    r = cdt[p1]<<16;
    return (b+r) << 1;
}
