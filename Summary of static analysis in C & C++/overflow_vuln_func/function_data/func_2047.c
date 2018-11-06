uint64_t helper_addlv (uint64_t op1, uint64_t op2)
{
    uint64_t tmp = op1;
    op1 = (uint32_t)(op1 + op2);
    if (unlikely((tmp ^ op2 ^ (-1UL)) & (tmp ^ op1) & (1UL << 31))) {
        arith_excp(env, GETPC(), EXC_M_IOV, 0);
    }
    return op1;
}
