static void
test_opts_range_unvisited(void)
{
    intList *list = NULL;
    intList *tail;
    QemuOpts *opts;
    Visitor *v;

    opts = qemu_opts_parse(qemu_find_opts("userdef"), "ilist=0-2", false,
                           &error_abort);

    v = opts_visitor_new(opts);

    visit_start_struct(v, NULL, NULL, 0, &error_abort);

    /
    visit_start_list(v, "ilist", (GenericList **)&list, sizeof(*list),
                     &error_abort);
    tail = list;
    visit_type_int(v, NULL, &tail->value, &error_abort);
    g_assert_cmpint(tail->value, ==, 0);
    tail = (intList *)visit_next_list(v, (GenericList *)tail, sizeof(*list));
    g_assert(tail);
    visit_type_int(v, NULL, &tail->value, &error_abort);
    g_assert_cmpint(tail->value, ==, 1);
    tail = (intList *)visit_next_list(v, (GenericList *)tail, sizeof(*list));
    g_assert(tail);
    visit_check_list(v, &error_abort); /
    visit_end_list(v, (void **)&list);

    visit_check_struct(v, &error_abort);
    visit_end_struct(v, NULL);

    qapi_free_intList(list);
    visit_free(v);
    qemu_opts_del(opts);
}
