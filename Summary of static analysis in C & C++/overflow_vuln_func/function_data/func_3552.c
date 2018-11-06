static inline int decode_alpha_block(const SHQContext *s, GetBitContext *gb, uint8_t last_alpha[16], uint8_t *dest, int linesize)
{
    uint8_t block[128];
    int i = 0, x, y;

    memset(block, 0, sizeof(block));

    {
        OPEN_READER(re, gb);

        for ( ;; ) {
            int run, level;

            UPDATE_CACHE_LE(re, gb);
            GET_VLC(run, re, gb, ff_dc_alpha_run_vlc_le.table, ALPHA_VLC_BITS, 2);

            if (run == 128) break;
            i += run;
            if (i >= 128)
                return AVERROR_INVALIDDATA;

            UPDATE_CACHE_LE(re, gb);
            GET_VLC(level, re, gb, ff_dc_alpha_level_vlc_le.table, ALPHA_VLC_BITS, 2);
            block[i++] = level;
        }

        CLOSE_READER(re, gb);
    }

    for (y = 0; y < 8; y++) {
        for (x = 0; x < 16; x++) {
            last_alpha[x] -= block[y * 16 + x];
        }
        memcpy(dest, last_alpha, 16);
        dest += linesize;
    }

    return 0;
}
