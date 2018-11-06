#if defined(TARGET_PPC64)
void do_divdo (void)
{
    if (likely(!(((int64_t)T0 == INT64_MIN && (int64_t)T1 == -1ULL) ||
                 (int64_t)T1 == 0))) {
        xer_ov = 0;
        T0 = (int64_t)T0 / (int64_t)T1;
    } else {
        xer_so = 1;
        xer_ov = 1;
        T0 = (-1ULL) * ((uint64_t)T0 >> 63);
    }
}
