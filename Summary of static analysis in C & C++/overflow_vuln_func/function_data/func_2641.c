int bdrv_commit(BlockDriverState *bs)
{
    BlockDriver *drv = bs->drv;
    int64_t sector, total_sectors;
    int n, ro, open_flags;
    int ret = 0;
    uint8_t *buf;
    char filename[1024];

    if (!drv)
        return -ENOMEDIUM;
    
    if (!bs->backing_hd) {
        return -ENOTSUP;
    }

    if (bdrv_in_use(bs) || bdrv_in_use(bs->backing_hd)) {
        return -EBUSY;
    }

    ro = bs->backing_hd->read_only;
    strncpy(filename, bs->backing_hd->filename, sizeof(filename));
    open_flags =  bs->backing_hd->open_flags;

    if (ro) {
        if (bdrv_reopen(bs->backing_hd, open_flags | BDRV_O_RDWR, NULL)) {
            return -EACCES;
        }
    }

    total_sectors = bdrv_getlength(bs) >> BDRV_SECTOR_BITS;
    buf = g_malloc(COMMIT_BUF_SECTORS * BDRV_SECTOR_SIZE);

    for (sector = 0; sector < total_sectors; sector += n) {
        if (bdrv_is_allocated(bs, sector, COMMIT_BUF_SECTORS, &n)) {

            if (bdrv_read(bs, sector, buf, n) != 0) {
                ret = -EIO;
                goto ro_cleanup;
            }

            if (bdrv_write(bs->backing_hd, sector, buf, n) != 0) {
                ret = -EIO;
                goto ro_cleanup;
            }
        }
    }

    if (drv->bdrv_make_empty) {
        ret = drv->bdrv_make_empty(bs);
        bdrv_flush(bs);
    }

    /
    if (bs->backing_hd)
        bdrv_flush(bs->backing_hd);

ro_cleanup:
    g_free(buf);

    if (ro) {
        /
        bdrv_reopen(bs->backing_hd, open_flags & ~BDRV_O_RDWR, NULL);
    }

    return ret;
}
