int bdrv_get_dirty(BlockDriverState *bs, int64_t sector)
{
    int64_t chunk = sector / (int64_t)BDRV_SECTORS_PER_DIRTY_CHUNK;

    if (bs->dirty_bitmap != NULL &&
        (sector << BDRV_SECTOR_BITS) <= bdrv_getlength(bs)) {
        return bs->dirty_bitmap[chunk];
    } else {
        return 0;
    }
}
