static int enable_write_target(BDRVVVFATState *s)
{
    BlockDriver *bdrv_qcow;
    QEMUOptionParameter *options;
    int ret;
    int size = sector2cluster(s, s->sector_count);
    s->used_clusters = calloc(size, 1);

    array_init(&(s->commits), sizeof(commit_t));

    s->qcow_filename = g_malloc(1024);
    get_tmp_filename(s->qcow_filename, 1024);

    bdrv_qcow = bdrv_find_format("qcow");
    options = parse_option_parameters("", bdrv_qcow->create_options, NULL);
    set_option_parameter_int(options, BLOCK_OPT_SIZE, s->sector_count * 512);
    set_option_parameter(options, BLOCK_OPT_BACKING_FILE, "fat:");

    if (bdrv_create(bdrv_qcow, s->qcow_filename, options) < 0)
	return -1;

    s->qcow = bdrv_new("");
    if (s->qcow == NULL) {
        return -1;
    }

    ret = bdrv_open(s->qcow, s->qcow_filename,
            BDRV_O_RDWR | BDRV_O_CACHE_WB | BDRV_O_NO_FLUSH, bdrv_qcow);
    if (ret < 0) {
	return ret;
    }

#ifndef _WIN32
    unlink(s->qcow_filename);
#endif

    s->bs->backing_hd = calloc(sizeof(BlockDriverState), 1);
    s->bs->backing_hd->drv = &vvfat_write_target;
    s->bs->backing_hd->opaque = g_malloc(sizeof(void*));
    *(void**)s->bs->backing_hd->opaque = s;

    return 0;
}
