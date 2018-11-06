void do_subfco (void)
{
    T2 = T0;
    T0 = T1 - T0;
    if (likely(T0 > T1)) {
        xer_ca = 0;
    } else {
        xer_ca = 1;
    }
    if (likely(!(((~T2) ^ T1 ^ (-1)) & ((~T2) ^ T0) & (1 << 31)))) {
        xer_ov = 0;
    } else {
        xer_so = 1;
        xer_ov = 1;
    }
}
