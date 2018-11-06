static float wv_get_value_float(WavpackFrameContext *s, uint32_t *crc, int S)
{
    union {
        float    f;
        uint32_t u;
    } value;

    unsigned int sign;
    int exp = s->float_max_exp;

    if (s->got_extra_bits) {
        const int max_bits  = 1 + 23 + 8 + 1;
        const int left_bits = get_bits_left(&s->gb_extra_bits);

        if (left_bits + 8 * AV_INPUT_BUFFER_PADDING_SIZE < max_bits)
            return 0.0;
    }

    if (S) {
        S  *= 1 << s->float_shift;
        sign = S < 0;
        if (sign)
            S = -S;
        if (S >= 0x1000000) {
            if (s->got_extra_bits && get_bits1(&s->gb_extra_bits))
                S = get_bits(&s->gb_extra_bits, 23);
            else
                S = 0;
            exp = 255;
        } else if (exp) {
            int shift = 23 - av_log2(S);
            exp = s->float_max_exp;
            if (exp <= shift)
                shift = --exp;
            exp -= shift;

            if (shift) {
                S <<= shift;
                if ((s->float_flag & WV_FLT_SHIFT_ONES) ||
                    (s->got_extra_bits &&
                     (s->float_flag & WV_FLT_SHIFT_SAME) &&
                     get_bits1(&s->gb_extra_bits))) {
                    S |= (1 << shift) - 1;
                } else if (s->got_extra_bits &&
                           (s->float_flag & WV_FLT_SHIFT_SENT)) {
                    S |= get_bits(&s->gb_extra_bits, shift);
                }
            }
        } else {
            exp = s->float_max_exp;
        }
        S &= 0x7fffff;
    } else {
        sign = 0;
        exp  = 0;
        if (s->got_extra_bits && (s->float_flag & WV_FLT_ZERO_SENT)) {
            if (get_bits1(&s->gb_extra_bits)) {
                S = get_bits(&s->gb_extra_bits, 23);
                if (s->float_max_exp >= 25)
                    exp = get_bits(&s->gb_extra_bits, 8);
                sign = get_bits1(&s->gb_extra_bits);
            } else {
                if (s->float_flag & WV_FLT_ZERO_SIGN)
                    sign = get_bits1(&s->gb_extra_bits);
            }
        }
    }

    *crc = *crc * 27 + S * 9 + exp * 3 + sign;

    value.u = (sign << 31) | (exp << 23) | S;
    return value.f;
}
