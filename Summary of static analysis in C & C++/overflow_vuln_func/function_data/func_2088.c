static void set_dirty_bitmap(BlockDriverState *bs, int64_t sector_num,
                             int nb_sectors, int dirty)
{
    int64_t start, end;

    start = sector_num / BDRV_SECTORS_PER_DIRTY_CHUNK;
    end = (sector_num + nb_sectors) / BDRV_SECTORS_PER_DIRTY_CHUNK;

    for (; start <= end; start++) {
        bs->dirty_bitmap[start] = dirty;
    }
}
