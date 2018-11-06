static int64_t allocate_clusters(BlockDriverState *bs, int64_t sector_num,
                                 int nb_sectors, int *pnum)
{
    BDRVParallelsState *s = bs->opaque;
    uint32_t idx, to_allocate, i;
    int64_t pos, space;

    pos = block_status(s, sector_num, nb_sectors, pnum);
    if (pos > 0) {
        return pos;
    }

    idx = sector_num / s->tracks;
    if (idx >= s->bat_size) {
        return -EINVAL;
    }

    to_allocate = DIV_ROUND_UP(sector_num + *pnum, s->tracks) - idx;
    space = to_allocate * s->tracks;
    if (s->data_end + space > bdrv_getlength(bs->file->bs) >> BDRV_SECTOR_BITS) {
        int ret;
        space += s->prealloc_size;
        if (s->prealloc_mode == PRL_PREALLOC_MODE_FALLOCATE) {
            ret = bdrv_pwrite_zeroes(bs->file,
                                     s->data_end << BDRV_SECTOR_BITS,
                                     space << BDRV_SECTOR_BITS, 0);
        } else {
            ret = bdrv_truncate(bs->file,
                                (s->data_end + space) << BDRV_SECTOR_BITS);
        }
        if (ret < 0) {
            return ret;
        }
    }

    for (i = 0; i < to_allocate; i++) {
        s->bat_bitmap[idx + i] = cpu_to_le32(s->data_end / s->off_multiplier);
        s->data_end += s->tracks;
        bitmap_set(s->bat_dirty_bmap,
                   bat_entry_off(idx + i) / s->bat_dirty_block, 1);
    }

    return bat2sect(s, idx) + sector_num % s->tracks;
}
