static void test_keyval_parse_list(void)
{
    Error *err = NULL;
    QDict *qdict, *sub_qdict;

    /
    qdict = keyval_parse("0=1", NULL, &err);
    error_free_or_abort(&err);
    g_assert(!qdict);

    /
    qdict = keyval_parse("list.0=null,list.2=zwei,list.1=eins",
                         NULL, &error_abort);
    g_assert_cmpint(qdict_size(qdict), ==, 1);
    check_list012(qdict_get_qlist(qdict, "list"));
    QDECREF(qdict);

    /
    qdict = keyval_parse("list.1=goner,list.0=null,list.1=eins,list.2=zwei",
                         NULL, &error_abort);
    g_assert_cmpint(qdict_size(qdict), ==, 1);
    check_list012(qdict_get_qlist(qdict, "list"));
    QDECREF(qdict);

    /
    qdict = keyval_parse("a.list.1=eins,a.list.0=null,a.list.2=zwei",
                         NULL, &error_abort);
    g_assert_cmpint(qdict_size(qdict), ==, 1);
    sub_qdict = qdict_get_qdict(qdict, "a");
    g_assert_cmpint(qdict_size(sub_qdict), ==, 1);
    check_list012(qdict_get_qlist(sub_qdict, "list"));
    QDECREF(qdict);

    /
    qdict = keyval_parse("a.b.c=1,a.b.0=2", NULL, &err);
    error_free_or_abort(&err);
    g_assert(!qdict);
    qdict = keyval_parse("a.0.c=1,a.b.c=2", NULL, &err);
    error_free_or_abort(&err);
    g_assert(!qdict);

    /
    qdict = keyval_parse("list.2=lonely", NULL, &err);
    error_free_or_abort(&err);
    g_assert(!qdict);
    qdict = keyval_parse("list.0=null,list.2=eins,list.02=zwei", NULL, &err);
    error_free_or_abort(&err);
    g_assert(!qdict);
}
