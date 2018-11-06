//very similar to MPEG-1
static inline int mdec_decode_block_intra(MDECContext *a, int16_t *block, int n)
{
    int level, diff, i, j, run;
    int component;
    RLTable *rl = &ff_rl_mpeg1;
    uint8_t * const scantable = a->scantable.permutated;
    const uint16_t *quant_matrix = ff_mpeg1_default_intra_matrix;
    const int qscale = a->qscale;

    /
    if (a->version == 2) {
        block[0] = 2 * get_sbits(&a->gb, 10) + 1024;
    } else {
        component = (n <= 3 ? 0 : n - 4 + 1);
        diff = decode_dc(&a->gb, component);
        if (diff >= 0xffff)
            return AVERROR_INVALIDDATA;
        a->last_dc[component] += diff;
        block[0] = a->last_dc[component] << 3;
    }

    i = 0;
    {
        OPEN_READER(re, &a->gb);
        /
        for (;;) {
            UPDATE_CACHE(re, &a->gb);
            GET_RL_VLC(level, run, re, &a->gb, rl->rl_vlc[0], TEX_VLC_BITS, 2, 0);

            if (level == 127) {
                break;
            } else if (level != 0) {
                i    += run;
                j     = scantable[i];
                level = (level * qscale * quant_matrix[j]) >> 3;
                level = (level ^ SHOW_SBITS(re, &a->gb, 1)) - SHOW_SBITS(re, &a->gb, 1);
                LAST_SKIP_BITS(re, &a->gb, 1);
            } else {
                /
                run = SHOW_UBITS(re, &a->gb, 6)+1; LAST_SKIP_BITS(re, &a->gb, 6);
                UPDATE_CACHE(re, &a->gb);
                level = SHOW_SBITS(re, &a->gb, 10); SKIP_BITS(re, &a->gb, 10);
                i    += run;
                j     = scantable[i];
                if (level < 0) {
                    level = -level;
                    level = (level * qscale * quant_matrix[j]) >> 3;
                    level = (level - 1) | 1;
                    level = -level;
                } else {
                    level = (level * qscale * quant_matrix[j]) >> 3;
                    level = (level - 1) | 1;
                }
            }
            if (i > 63) {
                av_log(a->avctx, AV_LOG_ERROR, "ac-tex damaged at %d %d\n", a->mb_x, a->mb_y);
                return AVERROR_INVALIDDATA;
            }

            block[j] = level;
        }
        CLOSE_READER(re, &a->gb);
    }
    a->block_last_index[n] = i;
    return 0;
}
