static inline int onenand_prog_spare(OneNANDState *s, int sec, int secn,
                void *src)
{
    int result = 0;
    if (secn > 0) {
        const uint8_t *sp = (const uint8_t *)src;
        uint8_t *dp = 0, *dpp = 0;
        if (s->blk_cur) {
            dp = g_malloc(512);
            if (!dp
                || blk_read(s->blk_cur, s->secs_cur + (sec >> 5), dp, 1) < 0) {
                result = 1;
            } else {
                dpp = dp + ((sec & 31) << 4);
            }
        } else {
            if (sec + secn > s->secs_cur) {
                result = 1;
            } else {
                dpp = s->current + (s->secs_cur << 9) + (sec << 4);
            }
        }
        if (!result) {
            uint32_t i;
            for (i = 0; i < (secn << 4); i++) {
                dpp[i] &= sp[i];
            }
            if (s->blk_cur) {
                result = blk_write(s->blk_cur, s->secs_cur + (sec >> 5),
                                   dp, 1) < 0;
            }
        }
        g_free(dp);
    }
    return result;
}
