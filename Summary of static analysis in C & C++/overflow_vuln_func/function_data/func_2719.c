static inline uint16_t mipsdsp_sat16_sub(int16_t a, int16_t b,
                                         CPUMIPSState *env)
{
    int16_t  temp;

    temp = a - b;
    if (MIPSDSP_OVERFLOW(a, -b, temp, 0x8000)) {
        if (a > 0) {
            temp = 0x7FFF;
        } else {
            temp = 0x8000;
        }
        set_DSPControl_overflow_flag(1, 20, env);
    }

    return temp;
}
