uint64_t helper_mullv(CPUAlphaState *env, uint64_t op1, uint64_t op2)
{
    int64_t res = (int64_t)op1 * (int64_t)op2;

    if (unlikely((int32_t)res != res)) {
        arith_excp(env, GETPC(), EXC_M_IOV, 0);
    }
    return (int64_t)((int32_t)res);
}
