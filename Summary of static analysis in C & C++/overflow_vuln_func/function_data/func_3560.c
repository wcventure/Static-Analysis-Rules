static int decode_555(GetByteContext *gB, uint16_t *dst, int stride,
                      int keyframe, int w, int h)
{
    int last_symbol = 0, repeat = 0, prev_avail = 0;

    if (!keyframe) {
        int x, y, endx, endy, t;

#define READ_PAIR(a, b)                 \
    a  = bytestream2_get_byte(gB) << 4; \
    t  = bytestream2_get_byte(gB);      \
    a |= t >> 4;                        \
    b  = (t & 0xF) << 8;                \
    b |= bytestream2_get_byte(gB);      \

        READ_PAIR(x, endx)
        READ_PAIR(y, endy)

        if (endx >= w || endy >= h || x > endx || y > endy)
            return AVERROR_INVALIDDATA;
        dst += x + stride * y;
        w    = endx - x + 1;
        h    = endy - y + 1;
        if (y)
            prev_avail = 1;
    }

    do {
        uint16_t *p = dst;
        do {
            if (repeat-- < 1) {
                int b = bytestream2_get_byte(gB);
                if (b < 128)
                    last_symbol = b << 8 | bytestream2_get_byte(gB);
                else if (b > 129) {
                    repeat = 0;
                    while (b-- > 130)
                        repeat = (repeat << 8) + bytestream2_get_byte(gB) + 1;
                    if (last_symbol == -2) {
                        int skip = FFMIN((unsigned)repeat, dst + w - p);
                        repeat -= skip;
                        p      += skip;
                    }
                } else
                    last_symbol = 127 - b;
            }
            if (last_symbol >= 0)
                *p = last_symbol;
            else if (last_symbol == -1 && prev_avail)
                *p = *(p - stride);
        } while (++p < dst + w);
        dst       += stride;
        prev_avail = 1;
    } while (--h);

    return 0;
}
