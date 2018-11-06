uint64_t HELPER(neon_sub_saturate_u64)(uint64_t src1, uint64_t src2)
{
    uint64_t res;

    if (src1 < src2) {
        env->QF = 1;
        res = 0;
    } else {
        res = src1 - src2;
    }
    return res;
}
