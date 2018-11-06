int bdrv_open_backing_file(BlockDriverState *bs, QDict *options, Error **errp)
{
    char backing_filename[PATH_MAX];
    int back_flags, ret;
    BlockDriver *back_drv = NULL;
    Error *local_err = NULL;

    if (bs->backing_hd != NULL) {
        QDECREF(options);
        return 0;
    }

    /
    if (options == NULL) {
        options = qdict_new();
    }

    bs->open_flags &= ~BDRV_O_NO_BACKING;
    if (qdict_haskey(options, "file.filename")) {
        backing_filename[0] = '\0';
    } else if (bs->backing_file[0] == '\0' && qdict_size(options) == 0) {
        QDECREF(options);
        return 0;
    } else {
        bdrv_get_full_backing_filename(bs, backing_filename,
                                       sizeof(backing_filename));
    }

    if (bs->backing_format[0] != '\0') {
        back_drv = bdrv_find_format(bs->backing_format);
    }

    /
    back_flags = bs->open_flags & ~(BDRV_O_RDWR | BDRV_O_SNAPSHOT |
                                    BDRV_O_COPY_ON_READ);

    assert(bs->backing_hd == NULL);
    ret = bdrv_open(&bs->backing_hd,
                    *backing_filename ? backing_filename : NULL, NULL, options,
                    back_flags, back_drv, &local_err);
    if (ret < 0) {
        bs->backing_hd = NULL;
        bs->open_flags |= BDRV_O_NO_BACKING;
        error_setg(errp, "Could not open backing file: %s",
                   error_get_pretty(local_err));
        error_free(local_err);
        return ret;
    }

    if (bs->backing_hd->file) {
        pstrcpy(bs->backing_file, sizeof(bs->backing_file),
                bs->backing_hd->file->filename);
    }

    /
    bdrv_refresh_limits(bs);

    return 0;
}
