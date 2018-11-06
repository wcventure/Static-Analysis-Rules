static void div64(uint64_t *plow, uint64_t *phigh, uint64_t b)
{
    uint64_t q, r, a1, a0;
    int i, qb;

    a0 = *plow;
    a1 = *phigh;
    if (a1 == 0) {
        q = a0 / b;
        r = a0 % b;
        *plow = q;
        *phigh = r;
    } else {
        /
        for(i = 0; i < 64; i++) {
            a1 = (a1 << 1) | (a0 >> 63);
            if (a1 >= b) {
                a1 -= b;
                qb = 1;
            } else {
                qb = 0;
            }
            a0 = (a0 << 1) | qb;
        }
#if defined(DEBUG_MULDIV)
        printf("div: 0x%016llx%016llx / 0x%016llx: q=0x%016llx r=0x%016llx\n",
               *phigh, *plow, b, a0, a1);
#endif
        *plow = a0;
        *phigh = a1;
    }
}
