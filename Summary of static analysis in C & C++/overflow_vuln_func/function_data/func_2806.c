static void test_opts_parse_size(void)
{
    Error *err = NULL;
    QemuOpts *opts;

    /
    opts = qemu_opts_parse(&opts_list_02, "size1=0", false, &error_abort);
    g_assert_cmpuint(opts_count(opts), ==, 1);
    g_assert_cmpuint(qemu_opt_get_size(opts, "size1", 1), ==, 0);

    /

    /
    opts = qemu_opts_parse(&opts_list_02,
                           "size1=9007199254740991,"
                           "size2=9007199254740992,"
                           "size3=9007199254740993",
                           false, &error_abort);
    g_assert_cmpuint(opts_count(opts), ==, 3);
    g_assert_cmphex(qemu_opt_get_size(opts, "size1", 1),
                     ==, 0x1fffffffffffff);
    g_assert_cmphex(qemu_opt_get_size(opts, "size2", 1),
                     ==, 0x20000000000000);
    g_assert_cmphex(qemu_opt_get_size(opts, "size3", 1),
                     ==, 0x20000000000000);

    /
    opts = qemu_opts_parse(&opts_list_02,
                           "size1=9223372036854774784," /
                           "size2=9223372036854775295", /
                           false, &error_abort);
    g_assert_cmpuint(opts_count(opts), ==, 2);
    g_assert_cmphex(qemu_opt_get_size(opts, "size1", 1),
                     ==, 0x7ffffffffffffc00);
    g_assert_cmphex(qemu_opt_get_size(opts, "size2", 1),
                     ==, 0x7ffffffffffffc00);

    /
    opts = qemu_opts_parse(&opts_list_02,
                           "size1=18446744073709549568," /
                           "size2=18446744073709550591", /
                           false, &error_abort);
    g_assert_cmpuint(opts_count(opts), ==, 2);
    g_assert_cmphex(qemu_opt_get_size(opts, "size1", 1),
                     ==, 0xfffffffffffff800);
    g_assert_cmphex(qemu_opt_get_size(opts, "size2", 1),
                     ==, 0xfffffffffffff800);

    /
    opts = qemu_opts_parse(&opts_list_02, "size1=-1", false, &err);
    error_free_or_abort(&err);
    g_assert(!opts);
    opts = qemu_opts_parse(&opts_list_02,
                           "size1=18446744073709550592", /
                           false, &error_abort);
    /
    g_assert_cmpuint(opts_count(opts), ==, 1);
    g_assert_cmpuint(qemu_opt_get_size(opts, "size1", 1), ==, 0);

    /
    opts = qemu_opts_parse(&opts_list_02, "size1=8b,size2=1.5k,size3=2M",
                           false, &error_abort);
    g_assert_cmpuint(opts_count(opts), ==, 3);
    g_assert_cmphex(qemu_opt_get_size(opts, "size1", 0), ==, 8);
    g_assert_cmphex(qemu_opt_get_size(opts, "size2", 0), ==, 1536);
    g_assert_cmphex(qemu_opt_get_size(opts, "size3", 0), ==, 2 * M_BYTE);
    opts = qemu_opts_parse(&opts_list_02, "size1=0.1G,size2=16777215T",
                           false, &error_abort);
    g_assert_cmpuint(opts_count(opts), ==, 2);
    g_assert_cmphex(qemu_opt_get_size(opts, "size1", 0), ==, G_BYTE / 10);
    g_assert_cmphex(qemu_opt_get_size(opts, "size2", 0),
                     ==, 16777215 * T_BYTE);

    /
    opts = qemu_opts_parse(&opts_list_02, "size1=16777216T",
                           false, &error_abort);
    /
    g_assert_cmpuint(opts_count(opts), ==, 1);
    g_assert_cmpuint(qemu_opt_get_size(opts, "size1", 1), ==, 0);

    /
    opts = qemu_opts_parse(&opts_list_02, "size1=16E", false, &err);
    error_free_or_abort(&err);
    g_assert(!opts);
    opts = qemu_opts_parse(&opts_list_02, "size1=16Gi", false, &error_abort);
    /
    g_assert_cmpuint(opts_count(opts), ==, 1);
    g_assert_cmpuint(qemu_opt_get_size(opts, "size1", 1), ==, 16 * G_BYTE);

    qemu_opts_reset(&opts_list_02);
}
