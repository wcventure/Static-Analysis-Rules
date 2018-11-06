uint64_t helper_sublv (uint64_t op1, uint64_t op2)
{
    uint64_t tmp = op1;
    op1 = (uint32_t)(op1 - op2);
    if (unlikely(((~tmp) ^ op1 ^ (-1UL)) & ((~tmp) ^ op2) & (1UL << 31))) {
        helper_excp(EXCP_ARITH, EXCP_ARITH_OVERFLOW);
    }
    return op1;
}
