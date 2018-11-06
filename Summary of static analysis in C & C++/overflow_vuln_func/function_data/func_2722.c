static inline int32_t mipsdsp_add_i32(int32_t a, int32_t b, CPUMIPSState *env)
{
    int32_t temp;

    temp = a + b;

    if (MIPSDSP_OVERFLOW(a, b, temp, 0x80000000)) {
        set_DSPControl_overflow_flag(1, 20, env);
    }

    return temp;
}
