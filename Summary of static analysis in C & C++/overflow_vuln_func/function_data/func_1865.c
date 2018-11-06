void qemu_iovec_init(QEMUIOVector *qiov, int alloc_hint)
{
    qiov->iov = g_malloc(alloc_hint * sizeof(struct iovec));
    qiov->niov = 0;
    qiov->nalloc = alloc_hint;
    qiov->size = 0;
}
