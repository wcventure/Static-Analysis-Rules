uint64_t helper_subqv(CPUAlphaState *env, uint64_t op1, uint64_t op2)
{
    uint64_t res;
    res = op1 - op2;
    if (unlikely((op1 ^ op2) & (res ^ op1) & (1ULL << 63))) {
        arith_excp(env, GETPC(), EXC_M_IOV, 0);
    }
    return res;
}
