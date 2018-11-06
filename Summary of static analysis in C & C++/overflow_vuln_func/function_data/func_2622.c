static inline uint16_t mipsdsp_trunc16_sat16_round(int32_t a,
                                                   CPUMIPSState *env)
{
    int64_t temp;

    temp = (int32_t)a + 0x00008000;

    if (a > (int)0x7fff8000) {
        temp = 0x7FFFFFFF;
        set_DSPControl_overflow_flag(1, 22, env);
    }

    return (temp >> 16) & 0xFFFF;
}
