static void qed_read_backing_file(BDRVQEDState *s, uint64_t pos,
                                  QEMUIOVector *qiov,
                                  BlockDriverCompletionFunc *cb, void *opaque)
{
    uint64_t backing_length = 0;
    size_t size;

    /
    if (s->bs->backing_hd) {
        int64_t l = bdrv_getlength(s->bs->backing_hd);
        if (l < 0) {
            cb(opaque, l);
            return;
        }
        backing_length = l;
    }

    /
    if (pos >= backing_length ||
        pos + qiov->size > backing_length) {
        qemu_iovec_memset(qiov, 0, 0, qiov->size);
    }

    /
    if (pos >= backing_length) {
        cb(opaque, 0);
        return;
    }

    /
    size = MIN((uint64_t)backing_length - pos, qiov->size);

    BLKDBG_EVENT(s->bs->file, BLKDBG_READ_BACKING_AIO);
    bdrv_aio_readv(s->bs->backing_hd, pos / BDRV_SECTOR_SIZE,
                   qiov, size / BDRV_SECTOR_SIZE, cb, opaque);
}
