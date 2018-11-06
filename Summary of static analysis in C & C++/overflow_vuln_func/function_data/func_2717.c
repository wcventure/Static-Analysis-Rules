static inline int32_t mipsdsp_sat_add_i32(int32_t a, int32_t b,
                                          CPUMIPSState *env)
{
    int32_t tempI;

    tempI = a + b;

    if (MIPSDSP_OVERFLOW(a, b, tempI, 0x80000000)) {
        if (a > 0) {
            tempI = 0x7FFFFFFF;
        } else {
            tempI = 0x80000000;
        }
        set_DSPControl_overflow_flag(1, 20, env);
    }

    return tempI;
}
