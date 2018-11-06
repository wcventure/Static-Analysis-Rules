static int raw_read_options(QDict *options, BlockDriverState *bs,
    BDRVRawState *s, Error **errp)
{
    Error *local_err = NULL;
    QemuOpts *opts = NULL;
    int64_t real_size = 0;
    int ret;

    real_size = bdrv_getlength(bs->file->bs);
    if (real_size < 0) {
        error_setg_errno(errp, -real_size, "Could not get image size");
        return real_size;
    }

    opts = qemu_opts_create(&raw_runtime_opts, NULL, 0, &error_abort);
    qemu_opts_absorb_qdict(opts, options, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        ret = -EINVAL;
        goto end;
    }

    s->offset = qemu_opt_get_size(opts, "offset", 0);
    if (qemu_opt_find(opts, "size") != NULL) {
        s->size = qemu_opt_get_size(opts, "size", 0);
        s->has_size = true;
    } else {
        s->has_size = false;
        s->size = real_size - s->offset;
    }

    /
    if (real_size < s->offset || (real_size - s->offset) < s->size) {
        error_setg(errp, "The sum of offset (%" PRIu64 ") and size "
            "(%" PRIu64 ") has to be smaller or equal to the "
            " actual size of the containing file (%" PRId64 ")",
            s->offset, s->size, real_size);
        ret = -EINVAL;
        goto end;
    }

    /
    if (!QEMU_IS_ALIGNED(s->size, BDRV_SECTOR_SIZE)) {
        error_setg(errp, "Specified size is not multiple of %llu",
            BDRV_SECTOR_SIZE);
        ret = -EINVAL;
        goto end;
    }

    ret = 0;

end:

    qemu_opts_del(opts);

    return ret;
}
