void do_subfzeo (void)
{
    T1 = T0;
    T0 = ~T0 + xer_ca;
    if (likely(!((~T1 ^ (-1)) & ((~T1) ^ T0) & (1 << 31)))) {
        xer_ov = 0;
    } else {
        xer_ov = 1;
        xer_so = 1;
    }
    if (likely(T0 >= ~T1)) {
        xer_ca = 0;
    } else {
        xer_ca = 1;
    }
}
