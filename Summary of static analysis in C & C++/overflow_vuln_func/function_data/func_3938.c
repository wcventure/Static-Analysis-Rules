static void fill_table(uint8_t* table[256 + 2*YUVRGB_TABLE_HEADROOM], const int elemsize,
                       const int inc, void *y_tab)
{
    int i;
    uint8_t *y_table = y_tab;

    y_table -= elemsize * (inc >> 9);

    for (i = 0; i < 256 + 2*YUVRGB_TABLE_HEADROOM; i++) {
        int64_t cb = av_clip(i-YUVRGB_TABLE_HEADROOM, 0, 255)*inc;
        table[i] = y_table + elemsize * (cb >> 16);
    }
}
