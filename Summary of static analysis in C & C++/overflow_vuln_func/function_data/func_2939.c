static int parallels_open(BlockDriverState *bs, QDict *options, int flags,
                          Error **errp)
{
    BDRVParallelsState *s = bs->opaque;
    int i;
    struct parallels_header ph;
    int ret;

    bs->read_only = 1; // no write support yet

    ret = bdrv_pread(bs->file, 0, &ph, sizeof(ph));
    if (ret < 0) {
        goto fail;
    }

    bs->total_sectors = le64_to_cpu(ph.nb_sectors);

    if (le32_to_cpu(ph.version) != HEADER_VERSION) {
        goto fail_format;
    }
    if (!memcmp(ph.magic, HEADER_MAGIC, 16)) {
        s->off_multiplier = 1;
        bs->total_sectors = 0xffffffff & bs->total_sectors;
    } else if (!memcmp(ph.magic, HEADER_MAGIC2, 16)) {
        s->off_multiplier = le32_to_cpu(ph.tracks);
    } else {
        goto fail_format;
    }

    s->tracks = le32_to_cpu(ph.tracks);
    if (s->tracks == 0) {
        error_setg(errp, "Invalid image: Zero sectors per track");
        ret = -EINVAL;
        goto fail;
    }
    if (s->tracks > INT32_MAX/513) {
        error_setg(errp, "Invalid image: Too big cluster");
        ret = -EFBIG;
        goto fail;
    }

    s->catalog_size = le32_to_cpu(ph.catalog_entries);
    if (s->catalog_size > INT_MAX / 4) {
        error_setg(errp, "Catalog too large");
        ret = -EFBIG;
        goto fail;
    }
    s->catalog_bitmap = g_try_malloc(s->catalog_size * 4);
    if (s->catalog_size && s->catalog_bitmap == NULL) {
        ret = -ENOMEM;
        goto fail;
    }

    ret = bdrv_pread(bs->file, 64, s->catalog_bitmap, s->catalog_size * 4);
    if (ret < 0) {
        goto fail;
    }

    for (i = 0; i < s->catalog_size; i++)
        le32_to_cpus(&s->catalog_bitmap[i]);

    qemu_co_mutex_init(&s->lock);
    return 0;

fail_format:
    error_setg(errp, "Image not in Parallels format");
    ret = -EINVAL;
fail:
    g_free(s->catalog_bitmap);
    return ret;
}
