static int coroutine_fn iscsi_co_readv(BlockDriverState *bs,
                                       int64_t sector_num, int nb_sectors,
                                       QEMUIOVector *iov)
{
    IscsiLun *iscsilun = bs->opaque;
    struct IscsiTask iTask;
    uint64_t lba;
    uint32_t num_sectors;

    if (!is_sector_request_lun_aligned(sector_num, nb_sectors, iscsilun)) {
        return -EINVAL;
    }

    if (bs->bl.max_transfer_length && nb_sectors > bs->bl.max_transfer_length) {
        error_report("iSCSI Error: Read of %d sectors exceeds max_xfer_len "
                     "of %d sectors", nb_sectors, bs->bl.max_transfer_length);
        return -EINVAL;
    }

    if (iscsilun->lbprz && nb_sectors >= ISCSI_CHECKALLOC_THRES &&
        !iscsi_allocationmap_is_allocated(iscsilun, sector_num, nb_sectors)) {
        int64_t ret;
        int pnum;
        BlockDriverState *file;
        ret = iscsi_co_get_block_status(bs, sector_num, INT_MAX, &pnum, &file);
        if (ret < 0) {
            return ret;
        }
        if (ret & BDRV_BLOCK_ZERO && pnum >= nb_sectors) {
            qemu_iovec_memset(iov, 0, 0x00, iov->size);
            return 0;
        }
    }

    lba = sector_qemu2lun(sector_num, iscsilun);
    num_sectors = sector_qemu2lun(nb_sectors, iscsilun);

    iscsi_co_init_iscsitask(iscsilun, &iTask);
retry:
    if (iscsilun->use_16_for_rw) {
        iTask.task = iscsi_read16_task(iscsilun->iscsi, iscsilun->lun, lba,
                                       num_sectors * iscsilun->block_size,
                                       iscsilun->block_size, 0, 0, 0, 0, 0,
                                       iscsi_co_generic_cb, &iTask);
    } else {
        iTask.task = iscsi_read10_task(iscsilun->iscsi, iscsilun->lun, lba,
                                       num_sectors * iscsilun->block_size,
                                       iscsilun->block_size,
                                       0, 0, 0, 0, 0,
                                       iscsi_co_generic_cb, &iTask);
    }
    if (iTask.task == NULL) {
        return -ENOMEM;
    }
    scsi_task_set_iov_in(iTask.task, (struct scsi_iovec *) iov->iov, iov->niov);

    while (!iTask.complete) {
        iscsi_set_events(iscsilun);
        qemu_coroutine_yield();
    }

    if (iTask.task != NULL) {
        scsi_free_scsi_task(iTask.task);
        iTask.task = NULL;
    }

    if (iTask.do_retry) {
        iTask.complete = 0;
        goto retry;
    }

    if (iTask.status != SCSI_STATUS_GOOD) {
        return iTask.err_code;
    }

    return 0;
}
