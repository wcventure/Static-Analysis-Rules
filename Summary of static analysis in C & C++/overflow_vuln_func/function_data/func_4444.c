static av_always_inline void RENAME(decode_line)(FFV1Context *s, int w,
                                                 TYPE *sample[2],
                                                 int plane_index, int bits)
{
    PlaneContext *const p = &s->plane[plane_index];
    RangeCoder *const c   = &s->c;
    int x;
    int run_count = 0;
    int run_mode  = 0;
    int run_index = s->run_index;

    if (s->slice_coding_mode == 1) {
        int i;
        for (x = 0; x < w; x++) {
            int v = 0;
            for (i=0; i<bits; i++) {
                uint8_t state = 128;
                v += v + get_rac(c, &state);
            }
            sample[1][x] = v;
        }
        return;
    }

    for (x = 0; x < w; x++) {
        int diff, context, sign;

        context = RENAME(get_context)(p, sample[1] + x, sample[0] + x, sample[1] + x);
        if (context < 0) {
            context = -context;
            sign    = 1;
        } else
            sign = 0;

        av_assert2(context < p->context_count);

        if (s->ac != AC_GOLOMB_RICE) {
            diff = get_symbol_inline(c, p->state[context], 1);
        } else {
            if (context == 0 && run_mode == 0)
                run_mode = 1;

            if (run_mode) {
                if (run_count == 0 && run_mode == 1) {
                    if (get_bits1(&s->gb)) {
                        run_count = 1 << ff_log2_run[run_index];
                        if (x + run_count <= w)
                            run_index++;
                    } else {
                        if (ff_log2_run[run_index])
                            run_count = get_bits(&s->gb, ff_log2_run[run_index]);
                        else
                            run_count = 0;
                        if (run_index)
                            run_index--;
                        run_mode = 2;
                    }
                }
                run_count--;
                if (run_count < 0) {
                    run_mode  = 0;
                    run_count = 0;
                    diff      = get_vlc_symbol(&s->gb, &p->vlc_state[context],
                                               bits);
                    if (diff >= 0)
                        diff++;
                } else
                    diff = 0;
            } else
                diff = get_vlc_symbol(&s->gb, &p->vlc_state[context], bits);

            ff_dlog(s->avctx, "count:%d index:%d, mode:%d, x:%d pos:%d\n",
                    run_count, run_index, run_mode, x, get_bits_count(&s->gb));
        }

        if (sign)
            diff = -diff;

        sample[1][x] = av_mod_uintp2(RENAME(predict)(sample[1] + x, sample[0] + x) + diff, bits);
    }
    s->run_index = run_index;
}
