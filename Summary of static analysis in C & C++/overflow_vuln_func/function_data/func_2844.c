uint64_t HELPER(neon_sub_saturate_s64)(uint64_t src1, uint64_t src2)
{
    uint64_t res;

    res = src1 - src2;
    if (((res ^ src1) & SIGNBIT64) && ((src1 ^ src2) & SIGNBIT64)) {
        env->QF = 1;
        res = ((int64_t)src1 >> 63) ^ ~SIGNBIT64;
    }
    return res;
}
