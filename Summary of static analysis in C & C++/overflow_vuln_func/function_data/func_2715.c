static inline int16_t mipsdsp_add_i16(int16_t a, int16_t b, CPUMIPSState *env)
{
    int16_t tempI;

    tempI = a + b;

    if (MIPSDSP_OVERFLOW(a, b, tempI, 0x8000)) {
        set_DSPControl_overflow_flag(1, 20, env);
    }

    return tempI;
}
