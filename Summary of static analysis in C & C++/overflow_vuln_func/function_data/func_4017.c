int av_base64_decode(uint8_t *out, const char *in, int out_size)
{
    int i, v;
    uint8_t *dst = out;

    v = 0;
    for (i = 0; in[i] && in[i] != '='; i++) {
        unsigned int index= in[i]-43;
        if (index>=FF_ARRAY_ELEMS(map2) || map2[index] == 0xff)
            return -1;
        v = (v << 6) + map2[index];
        if (i & 3) {
            if (dst - out < out_size) {
                *dst++ = v >> (6 - 2 * (i & 3));
            }
        }
    }

    return dst - out;
}
