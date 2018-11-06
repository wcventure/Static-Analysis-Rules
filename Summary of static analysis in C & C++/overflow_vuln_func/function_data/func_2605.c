static int enable_write_target(BDRVVVFATState *s, Error **errp)
{
    BlockDriver *bdrv_qcow = NULL;
    QemuOpts *opts = NULL;
    int ret;
    int size = sector2cluster(s, s->sector_count);
    s->used_clusters = calloc(size, 1);

    array_init(&(s->commits), sizeof(commit_t));

    s->qcow_filename = g_malloc(1024);
    ret = get_tmp_filename(s->qcow_filename, 1024);
    if (ret < 0) {
        error_setg_errno(errp, -ret, "can't create temporary file");
        goto err;
    }

    bdrv_qcow = bdrv_find_format("qcow");
    opts = qemu_opts_create(bdrv_qcow->create_opts, NULL, 0, &error_abort);
    qemu_opt_set_number(opts, BLOCK_OPT_SIZE, s->sector_count * 512);
    qemu_opt_set(opts, BLOCK_OPT_BACKING_FILE, "fat:");

    ret = bdrv_create(bdrv_qcow, s->qcow_filename, opts, errp);
    qemu_opts_del(opts);
    if (ret < 0) {
        goto err;
    }

    s->qcow = NULL;
    ret = bdrv_open(&s->qcow, s->qcow_filename, NULL, NULL,
                    BDRV_O_RDWR | BDRV_O_CACHE_WB | BDRV_O_NO_FLUSH,
                    bdrv_qcow, errp);
    if (ret < 0) {
        goto err;
    }

#ifndef _WIN32
    unlink(s->qcow_filename);
#endif

    bdrv_set_backing_hd(s->bs, bdrv_new("", &error_abort));
    s->bs->backing_hd->drv = &vvfat_write_target;
    s->bs->backing_hd->opaque = g_malloc(sizeof(void*));
    *(void**)s->bs->backing_hd->opaque = s;

    return 0;

err:
    g_free(s->qcow_filename);
    s->qcow_filename = NULL;
    return ret;
}
