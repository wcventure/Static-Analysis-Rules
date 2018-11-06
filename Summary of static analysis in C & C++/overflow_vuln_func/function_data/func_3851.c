static void fill_gv_table(int table[256 + 2*YUVRGB_TABLE_HEADROOM], const int elemsize, const int inc)
{
    int i;
    int off    = -(inc >> 9);

    for (i = 0; i < 256 + 2*YUVRGB_TABLE_HEADROOM; i++) {
        int64_t cb = av_clip(i-YUVRGB_TABLE_HEADROOM, 0, 255)*inc;
        table[i] = elemsize * (off + (cb >> 16));
    }
}
