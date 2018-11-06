void do_addmeo (void)
{
    T1 = T0;
    T0 += xer_ca + (-1);
    if (likely(!((uint32_t)T1 &
                 ((uint32_t)T1 ^ (uint32_t)T0) & (1UL << 31)))) {
        xer_ov = 0;
    } else {
        xer_so = 1;
        xer_ov = 1;
    }
    if (likely(T1 != 0))
        xer_ca = 1;
}
