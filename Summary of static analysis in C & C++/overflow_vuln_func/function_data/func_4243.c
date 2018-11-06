int ff_wma_run_level_decode(AVCodecContext* avctx, GetBitContext* gb,
                            VLC *vlc,
                            const uint16_t *level_table, const uint16_t *run_table,
                            int version, WMACoef *ptr, int offset,
                            int num_coefs, int block_len, int frame_len_bits,
                            int coef_nb_bits)
{
    int code, run, level, sign;
    WMACoef* eptr = ptr + num_coefs;
    ptr += offset;
    for(;;) {
        code = get_vlc2(gb, vlc->table, VLCBITS, VLCMAX);
        if (code < 0)
            return -1;
        if (code == 1) {
            /
            break;
        } else if (code == 0) {
            /
            if (!version) {
                level = get_bits(gb, coef_nb_bits);
                /
                run = get_bits(gb, frame_len_bits);
            } else {
                level = ff_wma_get_large_val(gb);
                /
                if (get_bits1(gb)) {
                    if (get_bits1(gb)) {
                        if (get_bits1(gb)) {
                            av_log(avctx,AV_LOG_ERROR,
                                "broken escape sequence\n");
                            return -1;
                        } else
                            run = get_bits(gb, frame_len_bits) + 4;
                    } else
                        run = get_bits(gb, 2) + 1;
                } else
                     run = 0;
            }
        } else {
            /
            run = run_table[code];
            level = level_table[code];
        }
        sign = get_bits1(gb);
        if (!sign)
             level = -level;
        ptr += run;
        if (ptr >= eptr)
        {
            av_log(NULL, AV_LOG_ERROR, "overflow in spectral RLE, ignoring\n");
            break;
        }
        *ptr++ = level;
        /
        if (ptr >= eptr)
            break;
    }
    return 0;
}
