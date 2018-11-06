static void
opts_check_struct(Visitor *v, Error **errp)
{
    OptsVisitor *ov = to_ov(v);
    GHashTableIter iter;
    GQueue *any;

    if (ov->depth > 0) {
        return;
    }

    /
    g_hash_table_iter_init(&iter, ov->unprocessed_opts);
    if (g_hash_table_iter_next(&iter, NULL, (void **)&any)) {
        const QemuOpt *first;

        first = g_queue_peek_head(any);
        error_setg(errp, QERR_INVALID_PARAMETER, first->name);
    }
}
