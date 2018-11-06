int qcow2_snapshot_goto(BlockDriverState *bs, const char *snapshot_id)
{
    BDRVQcowState *s = bs->opaque;
    QCowSnapshot *sn;
    int i, snapshot_index;
    int cur_l1_bytes, sn_l1_bytes;
    int ret;
    uint64_t *sn_l1_table = NULL;

    /
    snapshot_index = find_snapshot_by_id_or_name(bs, snapshot_id);
    if (snapshot_index < 0) {
        return -ENOENT;
    }
    sn = &s->snapshots[snapshot_index];

    if (sn->disk_size != bs->total_sectors * BDRV_SECTOR_SIZE) {
        error_report("qcow2: Loading snapshots with different disk "
            "size is not implemented");
        ret = -ENOTSUP;
        goto fail;
    }

    /
    ret = qcow2_grow_l1_table(bs, sn->l1_size, true);
    if (ret < 0) {
        goto fail;
    }

    cur_l1_bytes = s->l1_size * sizeof(uint64_t);
    sn_l1_bytes = sn->l1_size * sizeof(uint64_t);

    /
    sn_l1_table = g_malloc0(cur_l1_bytes);

    ret = bdrv_pread(bs->file, sn->l1_table_offset, sn_l1_table, sn_l1_bytes);
    if (ret < 0) {
        goto fail;
    }

    ret = qcow2_update_snapshot_refcount(bs, sn->l1_table_offset,
                                         sn->l1_size, 1);
    if (ret < 0) {
        goto fail;
    }

    ret = qcow2_pre_write_overlap_check(bs,
            QCOW2_OL_DEFAULT & ~QCOW2_OL_ACTIVE_L1,
            s->l1_table_offset, cur_l1_bytes);
    if (ret < 0) {
        goto fail;
    }

    ret = bdrv_pwrite_sync(bs->file, s->l1_table_offset, sn_l1_table,
                           cur_l1_bytes);
    if (ret < 0) {
        goto fail;
    }

    /
    ret = qcow2_update_snapshot_refcount(bs, s->l1_table_offset,
                                         s->l1_size, -1);

    /
    for(i = 0;i < s->l1_size; i++) {
        s->l1_table[i] = be64_to_cpu(sn_l1_table[i]);
    }

    if (ret < 0) {
        goto fail;
    }

    g_free(sn_l1_table);
    sn_l1_table = NULL;

    /
    ret = qcow2_update_snapshot_refcount(bs, s->l1_table_offset, s->l1_size, 0);
    if (ret < 0) {
        goto fail;
    }

#ifdef DEBUG_ALLOC
    {
        BdrvCheckResult result = {0};
        qcow2_check_refcounts(bs, &result, 0);
    }
#endif
    return 0;

fail:
    g_free(sn_l1_table);
    return ret;
}
