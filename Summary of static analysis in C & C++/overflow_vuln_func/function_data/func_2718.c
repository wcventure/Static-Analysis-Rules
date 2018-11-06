static inline uint16_t mipsdsp_sub_i16(int16_t a, int16_t b, CPUMIPSState *env)
{
    int16_t  temp;

    temp = a - b;
    if (MIPSDSP_OVERFLOW(a, -b, temp, 0x8000)) {
        set_DSPControl_overflow_flag(1, 20, env);
    }

    return temp;
}
