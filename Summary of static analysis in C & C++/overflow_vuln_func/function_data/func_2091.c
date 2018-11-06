BlockDriverAIOCB *bdrv_aio_writev(BlockDriverState *bs, int64_t sector_num,
                                  QEMUIOVector *qiov, int nb_sectors,
                                  BlockDriverCompletionFunc *cb, void *opaque)
{
    BlockDriver *drv = bs->drv;
    BlockDriverAIOCB *ret;

    if (!drv)
        return NULL;
    if (bs->read_only)
        return NULL;
    if (bdrv_check_request(bs, sector_num, nb_sectors))
        return NULL;

    if (bs->dirty_tracking) {
        set_dirty_bitmap(bs, sector_num, nb_sectors, 1);
    }

    ret = drv->bdrv_aio_writev(bs, sector_num, qiov, nb_sectors,
                               cb, opaque);

    if (ret) {
	/
	bs->wr_bytes += (unsigned) nb_sectors * BDRV_SECTOR_SIZE;
	bs->wr_ops ++;
    }

    return ret;
}
