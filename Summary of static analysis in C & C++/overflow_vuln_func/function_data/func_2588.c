int qcow2_alloc_cluster_link_l2(BlockDriverState *bs, QCowL2Meta *m)
{
    BDRVQcowState *s = bs->opaque;
    int i, j = 0, l2_index, ret;
    uint64_t *old_cluster, *l2_table;
    uint64_t cluster_offset = m->alloc_offset;

    trace_qcow2_cluster_link_l2(qemu_coroutine_self(), m->nb_clusters);
    assert(m->nb_clusters > 0);

    old_cluster = g_try_malloc(m->nb_clusters * sizeof(uint64_t));
    if (old_cluster == NULL) {
        ret = -ENOMEM;
        goto err;
    }

    /
    ret = perform_cow(bs, m, &m->cow_start);
    if (ret < 0) {
        goto err;
    }

    ret = perform_cow(bs, m, &m->cow_end);
    if (ret < 0) {
        goto err;
    }

    /
    if (s->use_lazy_refcounts) {
        qcow2_mark_dirty(bs);
    }
    if (qcow2_need_accurate_refcounts(s)) {
        qcow2_cache_set_dependency(bs, s->l2_table_cache,
                                   s->refcount_block_cache);
    }

    ret = get_cluster_table(bs, m->offset, &l2_table, &l2_index);
    if (ret < 0) {
        goto err;
    }
    qcow2_cache_entry_mark_dirty(s->l2_table_cache, l2_table);

    assert(l2_index + m->nb_clusters <= s->l2_size);
    for (i = 0; i < m->nb_clusters; i++) {
        /
        if(l2_table[l2_index + i] != 0)
            old_cluster[j++] = l2_table[l2_index + i];

        l2_table[l2_index + i] = cpu_to_be64((cluster_offset +
                    (i << s->cluster_bits)) | QCOW_OFLAG_COPIED);
     }


    ret = qcow2_cache_put(bs, s->l2_table_cache, (void**) &l2_table);
    if (ret < 0) {
        goto err;
    }

    /
    if (j != 0) {
        for (i = 0; i < j; i++) {
            qcow2_free_any_clusters(bs, be64_to_cpu(old_cluster[i]), 1,
                                    QCOW2_DISCARD_NEVER);
        }
    }

    ret = 0;
err:
    g_free(old_cluster);
    return ret;
 }
