static int vmdk_init_tables(BlockDriverState *bs, VmdkExtent *extent,
                            Error **errp)
{
    int ret;
    int l1_size, i;

    /
    l1_size = extent->l1_size * sizeof(uint32_t);
    extent->l1_table = g_try_malloc(l1_size);
    if (l1_size && extent->l1_table == NULL) {
        return -ENOMEM;
    }

    ret = bdrv_pread(extent->file,
                     extent->l1_table_offset,
                     extent->l1_table,
                     l1_size);
    if (ret < 0) {
        error_setg_errno(errp, -ret,
                         "Could not read l1 table from extent '%s'",
                         extent->file->filename);
        goto fail_l1;
    }
    for (i = 0; i < extent->l1_size; i++) {
        le32_to_cpus(&extent->l1_table[i]);
    }

    if (extent->l1_backup_table_offset) {
        extent->l1_backup_table = g_try_malloc(l1_size);
        if (l1_size && extent->l1_backup_table == NULL) {
            ret = -ENOMEM;
            goto fail_l1;
        }
        ret = bdrv_pread(extent->file,
                         extent->l1_backup_table_offset,
                         extent->l1_backup_table,
                         l1_size);
        if (ret < 0) {
            error_setg_errno(errp, -ret,
                             "Could not read l1 backup table from extent '%s'",
                             extent->file->filename);
            goto fail_l1b;
        }
        for (i = 0; i < extent->l1_size; i++) {
            le32_to_cpus(&extent->l1_backup_table[i]);
        }
    }

    extent->l2_cache =
        g_new(uint32_t, extent->l2_size * L2_CACHE_SIZE);
    return 0;
 fail_l1b:
    g_free(extent->l1_backup_table);
 fail_l1:
    g_free(extent->l1_table);
    return ret;
}
