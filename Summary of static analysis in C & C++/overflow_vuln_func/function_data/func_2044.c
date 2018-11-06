static void usbredir_put_bufpq(QEMUFile *f, void *priv, size_t unused)
{
    struct endp_data *endp = priv;
    USBRedirDevice *dev = endp->dev;
    struct buf_packet *bufp;
    int i = 0;

    qemu_put_be32(f, endp->bufpq_size);
    QTAILQ_FOREACH(bufp, &endp->bufpq, next) {
        DPRINTF("put_bufpq %d/%d len %d status %d\n", i + 1, endp->bufpq_size,
                bufp->len, bufp->status);
        qemu_put_be32(f, bufp->len);
        qemu_put_be32(f, bufp->status);
        qemu_put_buffer(f, bufp->data, bufp->len);
        i++;
    }
    assert(i == endp->bufpq_size);
}
