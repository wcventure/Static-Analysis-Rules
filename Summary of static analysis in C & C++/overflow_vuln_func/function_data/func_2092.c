void bdrv_set_dirty_tracking(BlockDriverState *bs, int enable)
{
    int64_t bitmap_size;

    if (enable) {
        if (bs->dirty_tracking == 0) {
            int64_t i;
            uint8_t test;

            bitmap_size = (bdrv_getlength(bs) >> BDRV_SECTOR_BITS);
            bitmap_size /= BDRV_SECTORS_PER_DIRTY_CHUNK;
            bitmap_size++;

            bs->dirty_bitmap = qemu_mallocz(bitmap_size);

            bs->dirty_tracking = enable;
            for(i = 0; i < bitmap_size; i++) test = bs->dirty_bitmap[i]; 
        }
    } else {
        if (bs->dirty_tracking != 0) {
            qemu_free(bs->dirty_bitmap);
            bs->dirty_tracking = enable;
        }
    }
}
