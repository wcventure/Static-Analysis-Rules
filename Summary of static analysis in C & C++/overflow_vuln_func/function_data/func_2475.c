static inline uint64_t cksm_overflow(uint64_t cksm)
{
    if (cksm > 0xffffffffULL) {
        cksm &= 0xffffffffULL;
        cksm++;
    }
    return cksm;
}
