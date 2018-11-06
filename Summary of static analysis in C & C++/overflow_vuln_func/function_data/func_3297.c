static inline int get_len(LZOContext *c, int x, int mask)
{
    int cnt = x & mask;
    if (!cnt) {
        while (!(x = get_byte(c)))
            cnt += 255;
        cnt += mask + x;
    }
    return cnt;
}
