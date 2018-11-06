static inline void decode_block_intra(MadContext *s, int16_t * block)
{
    int level, i, j, run;
    RLTable *rl = &ff_rl_mpeg1;
    const uint8_t *scantable = s->scantable.permutated;
    int16_t *quant_matrix = s->quant_matrix;

    block[0] = (128 + get_sbits(&s->gb, 8)) * quant_matrix[0];

    /
    i = 0;
    {
        OPEN_READER(re, &s->gb);
        /
        for (;;) {
            UPDATE_CACHE(re, &s->gb);
            GET_RL_VLC(level, run, re, &s->gb, rl->rl_vlc[0], TEX_VLC_BITS, 2, 0);

            if (level == 127) {
                break;
            } else if (level != 0) {
                i += run;
                j = scantable[i];
                level = (level*quant_matrix[j]) >> 4;
                level = (level-1)|1;
                level = (level ^ SHOW_SBITS(re, &s->gb, 1)) - SHOW_SBITS(re, &s->gb, 1);
                LAST_SKIP_BITS(re, &s->gb, 1);
            } else {
                /
                UPDATE_CACHE(re, &s->gb);
                level = SHOW_SBITS(re, &s->gb, 10); SKIP_BITS(re, &s->gb, 10);

                UPDATE_CACHE(re, &s->gb);
                run = SHOW_UBITS(re, &s->gb, 6)+1; LAST_SKIP_BITS(re, &s->gb, 6);

                i += run;
                j = scantable[i];
                if (level < 0) {
                    level = -level;
                    level = (level*quant_matrix[j]) >> 4;
                    level = (level-1)|1;
                    level = -level;
                } else {
                    level = (level*quant_matrix[j]) >> 4;
                    level = (level-1)|1;
                }
            }
            if (i > 63) {
                av_log(s->avctx, AV_LOG_ERROR, "ac-tex damaged at %d %d\n", s->mb_x, s->mb_y);
                return;
            }

            block[j] = level;
        }
        CLOSE_READER(re, &s->gb);
    }
}
