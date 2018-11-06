static int read_motion_values(AVCodecContext *avctx, GetBitContext *gb, Bundle *b)
{
    int t, sign, v;
    const uint8_t *dec_end;

    CHECK_READ_VAL(gb, b, t);
    dec_end = b->cur_dec + t;
    if (dec_end > b->data_end) {
        av_log(avctx, AV_LOG_ERROR, "Too many motion values\n");
        return -1;
    }
    if (get_bits1(gb)) {
        v = get_bits(gb, 4);
        if (v) {
            sign = -get_bits1(gb);
            v = (v ^ sign) - sign;
        }
        memset(b->cur_dec, v, t);
        b->cur_dec += t;
    } else {
        do {
            v = GET_HUFF(gb, b->tree);
            if (v) {
                sign = -get_bits1(gb);
                v = (v ^ sign) - sign;
            }
            *b->cur_dec++ = v;
        } while (b->cur_dec < dec_end);
    }
    return 0;
}
