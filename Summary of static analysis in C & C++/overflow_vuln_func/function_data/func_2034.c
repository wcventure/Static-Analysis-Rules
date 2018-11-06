static void hbitmap_test_init(TestHBitmapData *data,
                              uint64_t size, int granularity)
{
    size_t n;
    data->hb = hbitmap_alloc(size, granularity);

    n = (size + BITS_PER_LONG - 1) / BITS_PER_LONG;
    if (n == 0) {
        n = 1;
    }
    data->bits = g_new0(unsigned long, n);
    data->size = size;
    data->granularity = granularity;
    hbitmap_test_check(data, 0);
}
