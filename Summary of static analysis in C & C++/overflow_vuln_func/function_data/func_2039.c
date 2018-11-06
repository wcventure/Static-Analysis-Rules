static void usbredir_handle_bulk_data(USBRedirDevice *dev, USBPacket *p,
                                      uint8_t ep)
{
    struct usb_redir_bulk_packet_header bulk_packet;
    size_t size = (p->combined) ? p->combined->iov.size : p->iov.size;

    DPRINTF("bulk-out ep %02X len %zd id %"PRIu64"\n", ep, size, p->id);

    if (usbredir_already_in_flight(dev, p->id)) {
        p->status = USB_RET_ASYNC;
        return;
    }

    bulk_packet.endpoint  = ep;
    bulk_packet.length    = size;
    bulk_packet.stream_id = 0;
    bulk_packet.length_high = size >> 16;
    assert(bulk_packet.length_high == 0 ||
           usbredirparser_peer_has_cap(dev->parser,
                                       usb_redir_cap_32bits_bulk_length));

    if (ep & USB_DIR_IN) {
        usbredirparser_send_bulk_packet(dev->parser, p->id,
                                        &bulk_packet, NULL, 0);
    } else {
        uint8_t buf[size];
        if (p->combined) {
            iov_to_buf(p->combined->iov.iov, p->combined->iov.niov,
                       0, buf, size);
        } else {
            usb_packet_copy(p, buf, size);
        }
        usbredir_log_data(dev, "bulk data out:", buf, size);
        usbredirparser_send_bulk_packet(dev->parser, p->id,
                                        &bulk_packet, buf, size);
    }
    usbredirparser_do_write(dev->parser);
    p->status = USB_RET_ASYNC;
}
