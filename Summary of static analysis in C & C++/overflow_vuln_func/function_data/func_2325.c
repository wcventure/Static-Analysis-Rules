void do_subfmeo (void)
{
    T1 = T0;
    T0 = ~T0 + xer_ca - 1;
    if (likely(!(~T1 & (~T1 ^ T0) & (1 << 31)))) {
        xer_ov = 0;
    } else {
        xer_so = 1;
        xer_ov = 1;
    }
    if (likely(T1 != -1))
        xer_ca = 1;
}
