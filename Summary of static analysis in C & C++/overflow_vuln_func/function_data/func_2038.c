static void bufp_free(USBRedirDevice *dev, struct buf_packet *bufp,
    uint8_t ep)
{
    QTAILQ_REMOVE(&dev->endpoint[EP2I(ep)].bufpq, bufp, next);
    dev->endpoint[EP2I(ep)].bufpq_size--;
    free(bufp->data);
    g_free(bufp);
}
