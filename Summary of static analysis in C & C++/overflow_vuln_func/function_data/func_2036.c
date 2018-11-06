int main(int argc, char **argv)
{
    g_test_init(&argc, &argv, NULL);
    hbitmap_test_add("/hbitmap/size/0", test_hbitmap_zero);
    hbitmap_test_add("/hbitmap/size/unaligned", test_hbitmap_unaligned);
    hbitmap_test_add("/hbitmap/iter/empty", test_hbitmap_iter_empty);
    hbitmap_test_add("/hbitmap/iter/past", test_hbitmap_iter_past);
    hbitmap_test_add("/hbitmap/iter/partial", test_hbitmap_iter_partial);
    hbitmap_test_add("/hbitmap/iter/granularity", test_hbitmap_iter_granularity);
    hbitmap_test_add("/hbitmap/get/all", test_hbitmap_get_all);
    hbitmap_test_add("/hbitmap/get/some", test_hbitmap_get_some);
    hbitmap_test_add("/hbitmap/set/all", test_hbitmap_set_all);
    hbitmap_test_add("/hbitmap/set/one", test_hbitmap_set_one);
    hbitmap_test_add("/hbitmap/set/two-elem", test_hbitmap_set_two_elem);
    hbitmap_test_add("/hbitmap/set/general", test_hbitmap_set);
    hbitmap_test_add("/hbitmap/set/twice", test_hbitmap_set_twice);
    hbitmap_test_add("/hbitmap/set/overlap", test_hbitmap_set_overlap);
    hbitmap_test_add("/hbitmap/reset/empty", test_hbitmap_reset_empty);
    hbitmap_test_add("/hbitmap/reset/general", test_hbitmap_reset);
    hbitmap_test_add("/hbitmap/granularity", test_hbitmap_granularity);
    g_test_run();

    return 0;
}
