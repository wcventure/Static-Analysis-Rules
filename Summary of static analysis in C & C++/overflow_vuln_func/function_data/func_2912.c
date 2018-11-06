static int coroutine_fn bdrv_aligned_preadv(BlockDriverState *bs,
    BdrvTrackedRequest *req, int64_t offset, unsigned int bytes,
    int64_t align, QEMUIOVector *qiov, int flags)
{
    int64_t total_bytes, max_bytes;
    int ret;

    assert(is_power_of_2(align));
    assert((offset & (align - 1)) == 0);
    assert((bytes & (align - 1)) == 0);
    assert(!qiov || bytes == qiov->size);
    assert((bs->open_flags & BDRV_O_NO_IO) == 0);

    /
    assert(!(flags & ~(BDRV_REQ_NO_SERIALISING | BDRV_REQ_COPY_ON_READ)));

    /
    if (flags & BDRV_REQ_COPY_ON_READ) {
        /
        mark_request_serialising(req, bdrv_get_cluster_size(bs));
    }

    if (!(flags & BDRV_REQ_NO_SERIALISING)) {
        wait_serialising_requests(req);
    }

    if (flags & BDRV_REQ_COPY_ON_READ) {
        int64_t start_sector = offset >> BDRV_SECTOR_BITS;
        int64_t end_sector = DIV_ROUND_UP(offset + bytes, BDRV_SECTOR_SIZE);
        unsigned int nb_sectors = end_sector - start_sector;
        int pnum;

        ret = bdrv_is_allocated(bs, start_sector, nb_sectors, &pnum);
        if (ret < 0) {
            goto out;
        }

        if (!ret || pnum != nb_sectors) {
            ret = bdrv_co_do_copy_on_readv(bs, offset, bytes, qiov);
            goto out;
        }
    }

    /
    total_bytes = bdrv_getlength(bs);
    if (total_bytes < 0) {
        ret = total_bytes;
        goto out;
    }

    max_bytes = ROUND_UP(MAX(0, total_bytes - offset), align);
    if (bytes < max_bytes) {
        ret = bdrv_driver_preadv(bs, offset, bytes, qiov, 0);
    } else if (max_bytes > 0) {
        QEMUIOVector local_qiov;

        qemu_iovec_init(&local_qiov, qiov->niov);
        qemu_iovec_concat(&local_qiov, qiov, 0, max_bytes);

        ret = bdrv_driver_preadv(bs, offset, max_bytes, &local_qiov, 0);

        qemu_iovec_destroy(&local_qiov);
    } else {
        ret = 0;
    }

    /
    if (ret == 0 && total_bytes < offset + bytes) {
        uint64_t zero_offset = MAX(0, total_bytes - offset);
        uint64_t zero_bytes = offset + bytes - zero_offset;
        qemu_iovec_memset(qiov, zero_offset, 0, zero_bytes);
    }

out:
    return ret;
}
