static inline int onenand_prog_main(OneNANDState *s, int sec, int secn,
                void *src)
{
    int result = 0;

    if (secn > 0) {
        uint32_t size = (uint32_t)secn * 512;
        const uint8_t *sp = (const uint8_t *)src;
        uint8_t *dp = 0;
        if (s->blk_cur) {
            dp = g_malloc(size);
            if (!dp || blk_read(s->blk_cur, sec, dp, secn) < 0) {
                result = 1;
            }
        } else {
            if (sec + secn > s->secs_cur) {
                result = 1;
            } else {
                dp = (uint8_t *)s->current + (sec << 9);
            }
        }
        if (!result) {
            uint32_t i;
            for (i = 0; i < size; i++) {
                dp[i] &= sp[i];
            }
            if (s->blk_cur) {
                result = blk_write(s->blk_cur, sec, dp, secn) < 0;
            }
        }
        if (dp && s->blk_cur) {
            g_free(dp);
        }
    }

    return result;
}
