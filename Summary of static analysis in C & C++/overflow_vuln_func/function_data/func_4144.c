static inline av_flatten int get_symbol_inline(RangeCoder *c, uint8_t *state,
                                               int is_signed)
{
    if (get_rac(c, state + 0))
        return 0;
    else {
        int i, e, a;
        e = 0;
        while (get_rac(c, state + 1 + FFMIN(e, 9))) { // 1..10
            e++;
            if (e > 31)
                return AVERROR_INVALIDDATA;
        }

        a = 1;
        for (i = e - 1; i >= 0; i--)
            a += a + get_rac(c, state + 22 + FFMIN(i, 9));  // 22..31

        e = -(is_signed && get_rac(c, state + 11 + FFMIN(e, 10))); // 11..21
        return (a ^ e) - e;
    }
}
