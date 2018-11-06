uint64_t helper_mulqv (uint64_t op1, uint64_t op2)
{
    uint64_t tl, th;

    muls64(&tl, &th, op1, op2);
    /
    if (unlikely((th + 1) > 1)) {
        arith_excp(env, GETPC(), EXC_M_IOV, 0);
    }
    return tl;
}
