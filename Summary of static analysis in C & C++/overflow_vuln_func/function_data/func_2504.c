uint64_t helper_addqv(CPUAlphaState *env, uint64_t op1, uint64_t op2)
{
    uint64_t tmp = op1;
    op1 += op2;
    if (unlikely((tmp ^ op2 ^ (-1ULL)) & (tmp ^ op1) & (1ULL << 63))) {
        arith_excp(env, GETPC(), EXC_M_IOV, 0);
    }
    return op1;
}
