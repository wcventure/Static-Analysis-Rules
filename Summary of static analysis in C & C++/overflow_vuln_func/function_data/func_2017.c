static inline int onenand_load_spare(OneNANDState *s, int sec, int secn,
                void *dest)
{
    uint8_t buf[512];

    if (s->blk_cur) {
        if (blk_read(s->blk_cur, s->secs_cur + (sec >> 5), buf, 1) < 0) {
            return 1;
        }
        memcpy(dest, buf + ((sec & 31) << 4), secn << 4);
    } else if (sec + secn > s->secs_cur) {
        return 1;
    } else {
        memcpy(dest, s->current + (s->secs_cur << 9) + (sec << 4), secn << 4);
    }
 
    return 0;
}
