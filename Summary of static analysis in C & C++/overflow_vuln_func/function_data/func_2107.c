#if defined(TARGET_PPC64)
void do_addmeo_64 (void)
{
    T1 = T0;
    T0 += xer_ca + (-1);
    if (likely(!((uint64_t)T1 &
                 ((uint64_t)T1 ^ (uint64_t)T0) & (1ULL << 63)))) {
        xer_ov = 0;
    } else {
        xer_ov = 1;
        xer_so = 1;
    }
    if (likely(T1 != 0))
        xer_ca = 1;
}
