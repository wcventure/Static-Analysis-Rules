static inline int wv_get_value_integer(WavpackFrameContext *s, uint32_t *crc,
                                       int S)
{
    unsigned bit;

    if (s->extra_bits) {
        S *= 1 << s->extra_bits;

        if (s->got_extra_bits &&
            get_bits_left(&s->gb_extra_bits) >= s->extra_bits) {
            S   |= get_bits_long(&s->gb_extra_bits, s->extra_bits);
            *crc = *crc * 9 + (S & 0xffff) * 3 + ((unsigned)S >> 16);
        }
    }

    bit = (S & s->and) | s->or;
    bit = ((S + bit) << s->shift) - bit;

    if (s->hybrid)
        bit = av_clip(bit, s->hybrid_minclip, s->hybrid_maxclip);

    return bit << s->post_shift;
}
