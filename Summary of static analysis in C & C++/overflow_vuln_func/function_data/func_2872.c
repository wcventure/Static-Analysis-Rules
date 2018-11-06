static void qed_copy_from_backing_file(BDRVQEDState *s, uint64_t pos,
                                       uint64_t len, uint64_t offset,
                                       BlockDriverCompletionFunc *cb,
                                       void *opaque)
{
    CopyFromBackingFileCB *copy_cb;

    /
    if (len == 0) {
        cb(opaque, 0);
        return;
    }

    copy_cb = gencb_alloc(sizeof(*copy_cb), cb, opaque);
    copy_cb->s = s;
    copy_cb->offset = offset;
    copy_cb->iov.iov_base = qemu_blockalign(s->bs, len);
    copy_cb->iov.iov_len = len;
    qemu_iovec_init_external(&copy_cb->qiov, &copy_cb->iov, 1);

    qed_read_backing_file(s, pos, &copy_cb->qiov,
                          qed_copy_from_backing_file_write, copy_cb);
}
