static int block_save_iterate(QEMUFile *f, void *opaque)
{
    int ret;
    int64_t last_ftell = qemu_ftell(f);
    int64_t delta_ftell;

    DPRINTF("Enter save live iterate submitted %d transferred %d\n",
            block_mig_state.submitted, block_mig_state.transferred);

    ret = flush_blks(f);
    if (ret) {
        return ret;
    }

    blk_mig_reset_dirty_cursor();

    /
    blk_mig_lock();
    while ((block_mig_state.submitted +
            block_mig_state.read_done) * BLOCK_SIZE <
           qemu_file_get_rate_limit(f)) {
        blk_mig_unlock();
        if (block_mig_state.bulk_completed == 0) {
            /
            if (blk_mig_save_bulked_block(f) == 0) {
                /
                block_mig_state.bulk_completed = 1;
            }
            ret = 0;
        } else {
            /
            qemu_mutex_lock_iothread();
            ret = blk_mig_save_dirty_block(f, 1);
            qemu_mutex_unlock_iothread();
        }
        if (ret < 0) {
            return ret;
        }
        blk_mig_lock();
        if (ret != 0) {
            /
            break;
        }
    }
    blk_mig_unlock();

    ret = flush_blks(f);
    if (ret) {
        return ret;
    }

    qemu_put_be64(f, BLK_MIG_FLAG_EOS);
    delta_ftell = qemu_ftell(f) - last_ftell;
    if (delta_ftell > 0) {
        return 1;
    } else if (delta_ftell < 0) {
        return -1;
    } else {
        return 0;
    }
}
