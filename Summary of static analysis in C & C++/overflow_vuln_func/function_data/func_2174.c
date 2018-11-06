#if defined(TARGET_PPC64)
void do_divduo (void)
{
    if (likely((uint64_t)T1 != 0)) {
        xer_ov = 0;
        T0 = (uint64_t)T0 / (uint64_t)T1;
    } else {
        xer_so = 1;
        xer_ov = 1;
        T0 = 0;
    }
}
