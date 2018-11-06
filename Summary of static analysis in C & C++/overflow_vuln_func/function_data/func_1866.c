void qemu_iovec_add(QEMUIOVector *qiov, void *base, size_t len)
{
    assert(qiov->nalloc != -1);

    if (qiov->niov == qiov->nalloc) {
        qiov->nalloc = 2 * qiov->nalloc + 1;
        qiov->iov = g_realloc(qiov->iov, qiov->nalloc * sizeof(struct iovec));
    }
    qiov->iov[qiov->niov].iov_base = base;
    qiov->iov[qiov->niov].iov_len = len;
    qiov->size += len;
    ++qiov->niov;
}
