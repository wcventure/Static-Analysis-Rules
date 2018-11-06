static inline int onenand_erase(OneNANDState *s, int sec, int num)
{
    uint8_t *blankbuf, *tmpbuf;

    blankbuf = g_malloc(512);
    tmpbuf = g_malloc(512);
    memset(blankbuf, 0xff, 512);
    for (; num > 0; num--, sec++) {
        if (s->blk_cur) {
            int erasesec = s->secs_cur + (sec >> 5);
            if (blk_write(s->blk_cur, sec, blankbuf, 1) < 0) {
                goto fail;
            }
            if (blk_read(s->blk_cur, erasesec, tmpbuf, 1) < 0) {
                goto fail;
            }
            memcpy(tmpbuf + ((sec & 31) << 4), blankbuf, 1 << 4);
            if (blk_write(s->blk_cur, erasesec, tmpbuf, 1) < 0) {
                goto fail;
            }
        } else {
            if (sec + 1 > s->secs_cur) {
                goto fail;
            }
            memcpy(s->current + (sec << 9), blankbuf, 512);
            memcpy(s->current + (s->secs_cur << 9) + (sec << 4),
                   blankbuf, 1 << 4);
        }
    }

    g_free(tmpbuf);
    g_free(blankbuf);
    return 0;

fail:
    g_free(tmpbuf);
    g_free(blankbuf);
    return 1;
}
