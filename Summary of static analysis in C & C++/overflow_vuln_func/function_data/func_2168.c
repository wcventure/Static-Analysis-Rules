#if defined(TARGET_PPC64)
void OPPROTO op_check_subfo_64 (void)
{
    if (likely(!(((uint64_t)(~T2) ^ (uint64_t)T1 ^ UINT64_MAX) &
                 ((uint64_t)(~T2) ^ (uint64_t)T0) & (1ULL << 63)))) {
        xer_ov = 0;
    } else {
        xer_so = 1;
        xer_ov = 1;
    }
    RETURN();
}
