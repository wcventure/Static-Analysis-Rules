static void
test_opts_dict_unvisited(void)
{
    QemuOpts *opts;
    Visitor *v;
    UserDefOptions *userdef;

    opts = qemu_opts_parse(qemu_find_opts("userdef"), "i64x=0,bogus=1", false,
                           &error_abort);

    v = opts_visitor_new(opts);
    /
    visit_type_UserDefOptions(v, NULL, &userdef, &error_abort);
    visit_free(v);
    qemu_opts_del(opts);
    qapi_free_UserDefOptions(userdef);
}
