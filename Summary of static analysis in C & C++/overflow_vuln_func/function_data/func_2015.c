static inline int onenand_load_main(OneNANDState *s, int sec, int secn,
                void *dest)
{
    if (s->blk_cur) {
        return blk_read(s->blk_cur, sec, dest, secn) < 0;
    } else if (sec + secn > s->secs_cur) {
        return 1;
    }

    memcpy(dest, s->current + (sec << 9), secn << 9);

    return 0;
}
