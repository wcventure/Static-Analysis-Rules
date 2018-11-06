static int coroutine_fn copy_sectors(BlockDriverState *bs,
                                     uint64_t start_sect,
                                     uint64_t cluster_offset,
                                     int n_start, int n_end)
{
    BDRVQcowState *s = bs->opaque;
    QEMUIOVector qiov;
    struct iovec iov;
    int n, ret;

    /
    if (start_sect + n_end > bs->total_sectors) {
        n_end = bs->total_sectors - start_sect;
    }

    n = n_end - n_start;
    if (n <= 0) {
        return 0;
    }

    iov.iov_len = n * BDRV_SECTOR_SIZE;
    iov.iov_base = qemu_blockalign(bs, iov.iov_len);

    qemu_iovec_init_external(&qiov, &iov, 1);

    BLKDBG_EVENT(bs->file, BLKDBG_COW_READ);

    /
    ret = bs->drv->bdrv_co_readv(bs, start_sect + n_start, n, &qiov);
    if (ret < 0) {
        goto out;
    }

    if (s->crypt_method) {
        qcow2_encrypt_sectors(s, start_sect + n_start,
                        iov.iov_base, iov.iov_base, n, 1,
                        &s->aes_encrypt_key);
    }

    ret = qcow2_pre_write_overlap_check(bs, QCOW2_OL_DEFAULT,
            cluster_offset + n_start * BDRV_SECTOR_SIZE, n * BDRV_SECTOR_SIZE);
    if (ret < 0) {
        goto out;
    }

    BLKDBG_EVENT(bs->file, BLKDBG_COW_WRITE);
    ret = bdrv_co_writev(bs->file, (cluster_offset >> 9) + n_start, n, &qiov);
    if (ret < 0) {
        goto out;
    }

    ret = 0;
out:
    qemu_vfree(iov.iov_base);
    return ret;
}
