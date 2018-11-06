int bdrv_write_compressed(BlockDriverState *bs, int64_t sector_num,
                          const uint8_t *buf, int nb_sectors)
{
    BlockDriver *drv = bs->drv;
    if (!drv)
        return -ENOMEDIUM;
    if (!drv->bdrv_write_compressed)
        return -ENOTSUP;
    if (bdrv_check_request(bs, sector_num, nb_sectors))
        return -EIO;

    if (bs->dirty_tracking) {
        set_dirty_bitmap(bs, sector_num, nb_sectors, 1);
    }

    return drv->bdrv_write_compressed(bs, sector_num, buf, nb_sectors);
}
