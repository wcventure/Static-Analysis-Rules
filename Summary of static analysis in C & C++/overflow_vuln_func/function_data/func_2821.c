static int usbredir_get_bufpq(QEMUFile *f, void *priv, size_t unused)
{
    struct endp_data *endp = priv;
    USBRedirDevice *dev = endp->dev;
    struct buf_packet *bufp;
    int i;

    endp->bufpq_size = qemu_get_be32(f);
    for (i = 0; i < endp->bufpq_size; i++) {
        bufp = g_malloc(sizeof(struct buf_packet));
        bufp->len = qemu_get_be32(f);
        bufp->status = qemu_get_be32(f);
        bufp->offset = 0;
        bufp->data = qemu_oom_check(malloc(bufp->len)); /
        bufp->free_on_destroy = bufp->data;
        qemu_get_buffer(f, bufp->data, bufp->len);
        QTAILQ_INSERT_TAIL(&endp->bufpq, bufp, next);
        DPRINTF("get_bufpq %d/%d len %d status %d\n", i + 1, endp->bufpq_size,
                bufp->len, bufp->status);
    }
    return 0;
}
