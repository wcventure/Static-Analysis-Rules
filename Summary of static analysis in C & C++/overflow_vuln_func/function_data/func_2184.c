static int coroutine_fn bdrv_co_do_copy_on_readv(BdrvChild *child,
        int64_t offset, unsigned int bytes, QEMUIOVector *qiov)
{
    BlockDriverState *bs = child->bs;

    /
    void *bounce_buffer;

    BlockDriver *drv = bs->drv;
    struct iovec iov;
    QEMUIOVector local_qiov;
    int64_t cluster_offset;
    unsigned int cluster_bytes;
    size_t skip_bytes;
    int ret;
    int max_transfer = MIN_NON_ZERO(bs->bl.max_transfer,
                                    BDRV_REQUEST_MAX_BYTES);
    unsigned int progress = 0;

    /
    // assert(child->perm & (BLK_PERM_WRITE_UNCHANGED | BLK_PERM_WRITE));

    /
    bdrv_round_to_clusters(bs, offset, bytes, &cluster_offset, &cluster_bytes);
    skip_bytes = offset - cluster_offset;

    trace_bdrv_co_do_copy_on_readv(bs, offset, bytes,
                                   cluster_offset, cluster_bytes);

    bounce_buffer = qemu_try_blockalign(bs,
                                        MIN(MIN(max_transfer, cluster_bytes),
                                            MAX_BOUNCE_BUFFER));
    if (bounce_buffer == NULL) {
        ret = -ENOMEM;
        goto err;
    }

    while (cluster_bytes) {
        int64_t pnum;

        ret = bdrv_is_allocated(bs, cluster_offset,
                                MIN(cluster_bytes, max_transfer), &pnum);
        if (ret < 0) {
            /
            pnum = MIN(cluster_bytes, max_transfer);
        }

        assert(skip_bytes < pnum);

        if (ret <= 0) {
            /
            iov.iov_base = bounce_buffer;
            iov.iov_len = pnum = MIN(pnum, MAX_BOUNCE_BUFFER);
            qemu_iovec_init_external(&local_qiov, &iov, 1);

            ret = bdrv_driver_preadv(bs, cluster_offset, pnum,
                                     &local_qiov, 0);
            if (ret < 0) {
                goto err;
            }

            bdrv_debug_event(bs, BLKDBG_COR_WRITE);
            if (drv->bdrv_co_pwrite_zeroes &&
                buffer_is_zero(bounce_buffer, pnum)) {
                /
                ret = bdrv_co_do_pwrite_zeroes(bs, cluster_offset, pnum, 0);
            } else {
                /
                ret = bdrv_driver_pwritev(bs, cluster_offset, pnum,
                                          &local_qiov, 0);
            }

            if (ret < 0) {
                /
                goto err;
            }

            qemu_iovec_from_buf(qiov, progress, bounce_buffer + skip_bytes,
                                pnum - skip_bytes);
        } else {
            /
            qemu_iovec_init(&local_qiov, qiov->niov);
            qemu_iovec_concat(&local_qiov, qiov, progress, pnum - skip_bytes);
            ret = bdrv_driver_preadv(bs, offset + progress, local_qiov.size,
                                     &local_qiov, 0);
            qemu_iovec_destroy(&local_qiov);
            if (ret < 0) {
                goto err;
            }
        }

        cluster_offset += pnum;
        cluster_bytes -= pnum;
        progress += pnum - skip_bytes;
        skip_bytes = 0;
    }
    ret = 0;

err:
    qemu_vfree(bounce_buffer);
    return ret;
}
