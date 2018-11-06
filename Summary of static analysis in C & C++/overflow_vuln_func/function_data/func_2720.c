static inline uint32_t mipsdsp_sat32_sub(int32_t a, int32_t b,
                                         CPUMIPSState *env)
{
    int32_t  temp;

    temp = a - b;
    if (MIPSDSP_OVERFLOW(a, -b, temp, 0x80000000)) {
        if (a > 0) {
            temp = 0x7FFFFFFF;
        } else {
            temp = 0x80000000;
        }
        set_DSPControl_overflow_flag(1, 20, env);
    }

    return temp & 0xFFFFFFFFull;
}
