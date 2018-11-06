uint64_t helper_subqv (uint64_t op1, uint64_t op2)
{
    uint64_t tmp = op1;
    op1 -= op2;
    if (unlikely(((~tmp) ^ op1 ^ (-1ULL)) & ((~tmp) ^ op2) & (1ULL << 63))) {
        helper_excp(EXCP_ARITH, EXCP_ARITH_OVERFLOW);
    }
    return op1;
}
