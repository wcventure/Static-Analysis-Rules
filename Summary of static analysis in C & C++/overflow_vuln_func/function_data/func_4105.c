static inline void copy_backptr(LZOContext *c, int back, int cnt)
{
    register const uint8_t *src = &c->out[-back];
    register uint8_t *dst       = c->out;
    if (src < c->out_start || src > dst) {
        c->error |= AV_LZO_INVALID_BACKPTR;
        return;
    }
    if (cnt > c->out_end - dst) {
        cnt       = FFMAX(c->out_end - dst, 0);
        c->error |= AV_LZO_OUTPUT_FULL;
    }
    av_memcpy_backptr(dst, back, cnt);
    c->out = dst + cnt;
}
