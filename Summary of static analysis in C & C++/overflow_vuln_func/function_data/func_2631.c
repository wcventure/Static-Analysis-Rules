unsigned long hbitmap_iter_skip_words(HBitmapIter *hbi)
{
    size_t pos = hbi->pos;
    const HBitmap *hb = hbi->hb;
    unsigned i = HBITMAP_LEVELS - 1;

    unsigned long cur;
    do {
        cur = hbi->cur[--i];
        pos >>= BITS_PER_LEVEL;
    } while (cur == 0);

    /

    if (i == 0 && cur == (1UL << (BITS_PER_LONG - 1))) {
        return 0;
    }
    for (; i < HBITMAP_LEVELS - 1; i++) {
        /
        pos = (pos << BITS_PER_LEVEL) + ffsl(cur) - 1;
        hbi->cur[i] = cur & (cur - 1);

        /
        cur = hb->levels[i + 1][pos];
    }

    hbi->pos = pos;
    trace_hbitmap_iter_skip_words(hbi->hb, hbi, pos, cur);

    assert(cur);
    return cur;
}
