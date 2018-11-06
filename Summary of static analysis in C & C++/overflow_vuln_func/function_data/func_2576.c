BdrvDirtyBitmap *bdrv_create_dirty_bitmap(BlockDriverState *bs, int granularity,
                                          Error **errp)
{
    int64_t bitmap_size;
    BdrvDirtyBitmap *bitmap;

    assert((granularity & (granularity - 1)) == 0);

    granularity >>= BDRV_SECTOR_BITS;
    assert(granularity);
    bitmap_size = bdrv_nb_sectors(bs);
    if (bitmap_size < 0) {
        error_setg_errno(errp, -bitmap_size, "could not get length of device");
        errno = -bitmap_size;
        return NULL;
    }
    bitmap = g_malloc0(sizeof(BdrvDirtyBitmap));
    bitmap->bitmap = hbitmap_alloc(bitmap_size, ffs(granularity) - 1);
    QLIST_INSERT_HEAD(&bs->dirty_bitmaps, bitmap, list);
    return bitmap;
}
