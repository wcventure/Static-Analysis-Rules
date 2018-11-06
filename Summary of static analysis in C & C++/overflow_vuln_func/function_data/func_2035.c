static void test_hbitmap_iter_past(TestHBitmapData *data,
                                    const void *unused)
{
    hbitmap_test_init(data, L3, 0);
    hbitmap_test_set(data, 0, L3);
    hbitmap_test_check(data, L3);
}
