#if defined(TARGET_PPC64)
void do_subfmeo_64 (void)
{
    T1 = T0;
    T0 = ~T0 + xer_ca - 1;
    if (likely(!((uint64_t)~T1 & ((uint64_t)~T1 ^ (uint64_t)T0) &
                 (1ULL << 63)))) {
        xer_ov = 0;
    } else {
        xer_so = 1;
        xer_ov = 1;
    }
    if (likely((uint64_t)T1 != UINT64_MAX))
        xer_ca = 1;
}
