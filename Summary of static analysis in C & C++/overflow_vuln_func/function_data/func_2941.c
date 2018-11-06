int qed_check(BDRVQEDState *s, BdrvCheckResult *result, bool fix)
{
    QEDCheck check = {
        .s = s,
        .result = result,
        .nclusters = qed_bytes_to_clusters(s, s->file_size),
        .request = { .l2_table = NULL },
        .fix = fix,
    };
    int ret;

    check.used_clusters = g_try_malloc0(((check.nclusters + 31) / 32) *
                                        sizeof(check.used_clusters[0]));
    if (check.nclusters && check.used_clusters == NULL) {
        return -ENOMEM;
    }

    check.result->bfi.total_clusters =
        (s->header.image_size + s->header.cluster_size - 1) /
            s->header.cluster_size;
    ret = qed_check_l1_table(&check, s->l1_table);
    if (ret == 0) {
        /
        qed_check_for_leaks(&check);

        if (fix) {
            qed_check_mark_clean(s, result);
        }
    }

    g_free(check.used_clusters);
    return ret;
}
