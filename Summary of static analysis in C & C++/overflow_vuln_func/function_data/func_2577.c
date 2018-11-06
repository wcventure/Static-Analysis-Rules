BlockDirtyInfoList *bdrv_query_dirty_bitmaps(BlockDriverState *bs)
{
    BdrvDirtyBitmap *bm;
    BlockDirtyInfoList *list = NULL;
    BlockDirtyInfoList **plist = &list;

    QLIST_FOREACH(bm, &bs->dirty_bitmaps, list) {
        BlockDirtyInfo *info = g_malloc0(sizeof(BlockDirtyInfo));
        BlockDirtyInfoList *entry = g_malloc0(sizeof(BlockDirtyInfoList));
        info->count = bdrv_get_dirty_count(bs, bm);
        info->granularity =
            ((int64_t) BDRV_SECTOR_SIZE << hbitmap_granularity(bm->bitmap));
        entry->value = info;
        *plist = entry;
        plist = &entry->next;
    }

    return list;
}
