int bdrv_write(BlockDriverState *bs, int64_t sector_num,
               const uint8_t *buf, int nb_sectors)
{
    BlockDriver *drv = bs->drv;
    if (!bs->drv)
        return -ENOMEDIUM;
    if (bs->read_only)
        return -EACCES;
    if (bdrv_check_request(bs, sector_num, nb_sectors))
        return -EIO;

    if (bs->dirty_tracking) {
        set_dirty_bitmap(bs, sector_num, nb_sectors, 1);
    }

    return drv->bdrv_write(bs, sector_num, buf, nb_sectors);
}
