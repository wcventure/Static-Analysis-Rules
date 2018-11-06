void helper_##name(uint32_t ac, target_ulong rs, target_ulong rt,     \
                   CPUMIPSState *env)                                 \
{                                                                     \
    int32_t temp63;                                                   \
    int64_t dotp, acc;                                                \
    uint64_t temp;                                                    \
                                                                      \
    dotp = mipsdsp_mul_q31_q31(ac, rs, rt, env);                      \
    acc = ((uint64_t)env->active_tc.HI[ac] << 32) |                   \
          ((uint64_t)env->active_tc.LO[ac] & MIPSDSP_LLO);            \
    if (!is_add) {                                                    \
        dotp = -dotp;                                                 \
    }                                                                 \
                                                                      \
    temp = acc + dotp;                                                \
    if (MIPSDSP_OVERFLOW((uint64_t)acc, (uint64_t)dotp, temp,         \
                         (0x01ull << 63))) {                          \
        temp63 = (temp >> 63) & 0x01;                                 \
        if (temp63 == 1) {                                            \
            temp = (0x01ull << 63) - 1;                               \
        } else {                                                      \
            temp = 0x01ull << 63;                                     \
        }                                                             \
                                                                      \
        set_DSPControl_overflow_flag(1, 16 + ac, env);                \
    }                                                                 \
                                                                      \
    env->active_tc.HI[ac] = (target_long)(int32_t)                    \
        ((temp & MIPSDSP_LHI) >> 32);                                 \
    env->active_tc.LO[ac] = (target_long)(int32_t)                    \
        (temp & MIPSDSP_LLO);                                         \
}
