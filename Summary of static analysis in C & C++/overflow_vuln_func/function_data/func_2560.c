static void *qemu_fopen_rdma(RDMAContext *rdma, const char *mode)
{
    QEMUFileRDMA *r;

    if (qemu_file_mode_is_not_valid(mode)) {
        return NULL;
    }

    r = g_malloc0(sizeof(QEMUFileRDMA));
    r->rdma = rdma;

    if (mode[0] == 'w') {
        r->file = qemu_fopen_ops(r, &rdma_write_ops);
    } else {
        r->file = qemu_fopen_ops(r, &rdma_read_ops);
    }

    return r->file;
}
