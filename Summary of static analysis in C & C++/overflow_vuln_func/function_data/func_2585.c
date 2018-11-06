static int iscsi_create(const char *filename, QemuOpts *opts, Error **errp)
{
    int ret = 0;
    int64_t total_size = 0;
    BlockDriverState *bs;
    IscsiLun *iscsilun = NULL;
    QDict *bs_options;

    bs = bdrv_new("", &error_abort);

    /
    total_size =
        qemu_opt_get_size_del(opts, BLOCK_OPT_SIZE, 0) / BDRV_SECTOR_SIZE;
    bs->opaque = g_malloc0(sizeof(struct IscsiLun));
    iscsilun = bs->opaque;

    bs_options = qdict_new();
    qdict_put(bs_options, "filename", qstring_from_str(filename));
    ret = iscsi_open(bs, bs_options, 0, NULL);
    QDECREF(bs_options);

    if (ret != 0) {
        goto out;
    }
    iscsi_detach_aio_context(bs);
    if (iscsilun->type != TYPE_DISK) {
        ret = -ENODEV;
        goto out;
    }
    if (bs->total_sectors < total_size) {
        ret = -ENOSPC;
        goto out;
    }

    ret = 0;
out:
    if (iscsilun->iscsi != NULL) {
        iscsi_destroy_context(iscsilun->iscsi);
    }
    g_free(bs->opaque);
    bs->opaque = NULL;
    bdrv_unref(bs);
    return ret;
}
