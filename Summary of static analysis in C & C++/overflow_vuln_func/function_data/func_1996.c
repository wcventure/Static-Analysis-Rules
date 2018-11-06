static void test_opts_parse_number(void)
{
    Error *err = NULL;
    QemuOpts *opts;

    /
    opts = qemu_opts_parse(&opts_list_01, "number1=0", false, &error_abort);
    g_assert_cmpuint(opts_count(opts), ==, 1);
    g_assert_cmpuint(qemu_opt_get_number(opts, "number1", 1), ==, 0);

    /
    opts = qemu_opts_parse(&opts_list_01,
                           "number1=18446744073709551615,number2=-1",
                           false, &error_abort);
    g_assert_cmpuint(opts_count(opts), ==, 2);
    g_assert_cmphex(qemu_opt_get_number(opts, "number1", 1), ==, UINT64_MAX);
    g_assert_cmphex(qemu_opt_get_number(opts, "number2", 0), ==, UINT64_MAX);

    /
    opts = qemu_opts_parse(&opts_list_01, "number1=18446744073709551616",
                           false, &error_abort);
    /
    g_assert_cmpuint(opts_count(opts), ==, 1);
    g_assert_cmpuint(qemu_opt_get_number(opts, "number1", 1), ==, UINT64_MAX);

    /
    opts = qemu_opts_parse(&opts_list_01, "number1=-18446744073709551616",
                           false, &error_abort);
    /
    g_assert_cmpuint(opts_count(opts), ==, 1);
    g_assert_cmpuint(qemu_opt_get_number(opts, "number1", 1), ==, UINT64_MAX);

    /
    opts = qemu_opts_parse(&opts_list_01, "number1=0x2a,number2=052",
                           false, &error_abort);
    g_assert_cmpuint(opts_count(opts), ==, 2);
    g_assert_cmpuint(qemu_opt_get_number(opts, "number1", 1), ==, 42);
    g_assert_cmpuint(qemu_opt_get_number(opts, "number2", 0), ==, 42);

    /
    opts = qemu_opts_parse(&opts_list_01, "number1=", false, &err);
    /
    g_assert_cmpuint(opts_count(opts), ==, 1);
    g_assert_cmpuint(qemu_opt_get_number(opts, "number1", 1), ==, 0);
    opts = qemu_opts_parse(&opts_list_01, "number1=eins", false, &err);
    error_free_or_abort(&err);
    g_assert(!opts);

    /
    opts = qemu_opts_parse(&opts_list_01, "number1= \t42",
                           false, &error_abort);
    g_assert_cmpuint(opts_count(opts), ==, 1);
    g_assert_cmpuint(qemu_opt_get_number(opts, "number1", 1), ==, 42);

    /
    opts = qemu_opts_parse(&opts_list_01, "number1=3.14", false, &err);
    error_free_or_abort(&err);
    g_assert(!opts);
    opts = qemu_opts_parse(&opts_list_01, "number1=08", false, &err);
    error_free_or_abort(&err);
    g_assert(!opts);
    opts = qemu_opts_parse(&opts_list_01, "number1=0 ", false, &err);
    error_free_or_abort(&err);
    g_assert(!opts);

    qemu_opts_reset(&opts_list_01);
}
