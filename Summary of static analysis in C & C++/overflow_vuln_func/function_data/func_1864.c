HBitmap *hbitmap_alloc(uint64_t size, int granularity)
{
    HBitmap *hb = g_malloc0(sizeof (struct HBitmap));
    unsigned i;

    assert(granularity >= 0 && granularity < 64);
    size = (size + (1ULL << granularity) - 1) >> granularity;
    assert(size <= ((uint64_t)1 << HBITMAP_LOG_MAX_SIZE));

    hb->size = size;
    hb->granularity = granularity;
    for (i = HBITMAP_LEVELS; i-- > 0; ) {
        size = MAX((size + BITS_PER_LONG - 1) >> BITS_PER_LEVEL, 1);
        hb->levels[i] = g_malloc0(size * sizeof(unsigned long));
    }

    /
    assert(size == 1);
    hb->levels[0][0] |= 1UL << (BITS_PER_LONG - 1);
    return hb;
}
