static int mig_save_device_bulk(QEMUFile *f, BlkMigDevState *bmds)
{
    int64_t total_sectors = bmds->total_sectors;
    int64_t cur_sector = bmds->cur_sector;
    BlockDriverState *bs = bmds->bs;
    BlkMigBlock *blk;
    int nr_sectors;

    if (bmds->shared_base) {
        qemu_mutex_lock_iothread();
        while (cur_sector < total_sectors &&
               !bdrv_is_allocated(bs, cur_sector, MAX_IS_ALLOCATED_SEARCH,
                                  &nr_sectors)) {
            cur_sector += nr_sectors;
        }
        qemu_mutex_unlock_iothread();
    }

    if (cur_sector >= total_sectors) {
        bmds->cur_sector = bmds->completed_sectors = total_sectors;
        return 1;
    }

    bmds->completed_sectors = cur_sector;

    cur_sector &= ~((int64_t)BDRV_SECTORS_PER_DIRTY_CHUNK - 1);

    /
    nr_sectors = BDRV_SECTORS_PER_DIRTY_CHUNK;

    if (total_sectors - cur_sector < BDRV_SECTORS_PER_DIRTY_CHUNK) {
        nr_sectors = total_sectors - cur_sector;
    }

    blk = g_malloc(sizeof(BlkMigBlock));
    blk->buf = g_malloc(BLOCK_SIZE);
    blk->bmds = bmds;
    blk->sector = cur_sector;
    blk->nr_sectors = nr_sectors;

    blk->iov.iov_base = blk->buf;
    blk->iov.iov_len = nr_sectors * BDRV_SECTOR_SIZE;
    qemu_iovec_init_external(&blk->qiov, &blk->iov, 1);

    blk_mig_lock();
    block_mig_state.submitted++;
    blk_mig_unlock();

    qemu_mutex_lock_iothread();
    blk->aiocb = bdrv_aio_readv(bs, cur_sector, &blk->qiov,
                                nr_sectors, blk_mig_read_cb, blk);

    bdrv_reset_dirty(bs, cur_sector, nr_sectors);
    qemu_mutex_unlock_iothread();

    bmds->cur_sector = cur_sector + nr_sectors;
    return (bmds->cur_sector >= total_sectors);
}
