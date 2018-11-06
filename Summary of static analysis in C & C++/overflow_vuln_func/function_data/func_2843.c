uint64_t HELPER(neon_add_saturate_u64)(uint64_t src1, uint64_t src2)
{
    uint64_t res;

    res = src1 + src2;
    if (res < src1) {
        env->QF = 1;
        res = ~(uint64_t)0;
    }
    return res;
}
