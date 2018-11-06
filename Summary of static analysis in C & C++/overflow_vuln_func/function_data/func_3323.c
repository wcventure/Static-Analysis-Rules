static int read_dcs(AVCodecContext *avctx, GetBitContext *gb, Bundle *b,
                    int start_bits, int has_sign)
{
    int i, j, len, len2, bsize, sign, v, v2;
    int16_t *dst = (int16_t*)b->cur_dec;

    CHECK_READ_VAL(gb, b, len);
    v = get_bits(gb, start_bits - has_sign);
    if (v && has_sign) {
        sign = -get_bits1(gb);
        v = (v ^ sign) - sign;
    }
    *dst++ = v;
    len--;
    for (i = 0; i < len; i += 8) {
        len2 = FFMIN(len - i, 8);
        bsize = get_bits(gb, 4);
        if (bsize) {
            for (j = 0; j < len2; j++) {
                v2 = get_bits(gb, bsize);
                if (v2) {
                    sign = -get_bits1(gb);
                    v2 = (v2 ^ sign) - sign;
                }
                v += v2;
                *dst++ = v;
                if (v < -32768 || v > 32767) {
                    av_log(avctx, AV_LOG_ERROR, "DC value went out of bounds: %d\n", v);
                    return -1;
                }
            }
        } else {
            for (j = 0; j < len2; j++)
                *dst++ = v;
        }
    }

    b->cur_dec = (uint8_t*)dst;
    return 0;
}
