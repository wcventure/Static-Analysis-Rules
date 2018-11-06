static void nbd_refresh_limits(BlockDriverState *bs, Error **errp)
{
    bs->bl.max_discard = UINT32_MAX >> BDRV_SECTOR_BITS;
    bs->bl.max_transfer_length = UINT32_MAX >> BDRV_SECTOR_BITS;
}
