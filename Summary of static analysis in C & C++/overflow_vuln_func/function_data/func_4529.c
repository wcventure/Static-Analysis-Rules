static void vc1_mspel_mc(uint8_t *dst, const uint8_t *src, int stride, int mode, int rnd)
{
    int i, j;
    uint8_t tmp[8*11], *tptr;
    int m, r;

    m = (mode & 3);
    r = rnd;
    src -= stride;
    tptr = tmp;
    for(j = 0; j < 11; j++) {
        for(i = 0; i < 8; i++)
            tptr[i] = vc1_mspel_filter(src + i, 1, m, r);
        src += stride;
        tptr += 8;
    }
    r = 1 - rnd;
    m = (mode >> 2) & 3;

    tptr = tmp + 8;
    for(j = 0; j < 8; j++) {
        for(i = 0; i < 8; i++)
            dst[i] = vc1_mspel_filter(tptr + i, 8, m, r);
        dst += stride;
        tptr += 8;
    }
}
