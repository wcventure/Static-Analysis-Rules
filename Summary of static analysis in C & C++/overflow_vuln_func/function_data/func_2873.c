static void qed_aio_read_data(void *opaque, int ret,
                              uint64_t offset, size_t len)
{
    QEDAIOCB *acb = opaque;
    BDRVQEDState *s = acb_to_s(acb);
    BlockDriverState *bs = acb->common.bs;

    /
    offset += qed_offset_into_cluster(s, acb->cur_pos);

    trace_qed_aio_read_data(s, acb, ret, offset, len);

    if (ret < 0) {
        goto err;
    }

    qemu_iovec_concat(&acb->cur_qiov, acb->qiov, acb->qiov_offset, len);

    /
    if (ret == QED_CLUSTER_ZERO) {
        qemu_iovec_memset(&acb->cur_qiov, 0, 0, acb->cur_qiov.size);
        qed_aio_next_io(acb, 0);
        return;
    } else if (ret != QED_CLUSTER_FOUND) {
        qed_read_backing_file(s, acb->cur_pos, &acb->cur_qiov,
                              qed_aio_next_io, acb);
        return;
    }

    BLKDBG_EVENT(bs->file, BLKDBG_READ_AIO);
    bdrv_aio_readv(bs->file, offset / BDRV_SECTOR_SIZE,
                   &acb->cur_qiov, acb->cur_qiov.size / BDRV_SECTOR_SIZE,
                   qed_aio_next_io, acb);
    return;

err:
    qed_aio_complete(acb, ret);
}
