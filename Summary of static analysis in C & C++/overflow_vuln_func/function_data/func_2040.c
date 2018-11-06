static void usbredir_handle_data(USBDevice *udev, USBPacket *p)
{
    USBRedirDevice *dev = DO_UPCAST(USBRedirDevice, dev, udev);
    uint8_t ep;

    ep = p->ep->nr;
    if (p->pid == USB_TOKEN_IN) {
        ep |= USB_DIR_IN;
    }

    switch (dev->endpoint[EP2I(ep)].type) {
    case USB_ENDPOINT_XFER_CONTROL:
        ERROR("handle_data called for control transfer on ep %02X\n", ep);
        p->status = USB_RET_NAK;
        break;
    case USB_ENDPOINT_XFER_ISOC:
        usbredir_handle_iso_data(dev, p, ep);
        break;
    case USB_ENDPOINT_XFER_BULK:
        if (p->state == USB_PACKET_SETUP && p->pid == USB_TOKEN_IN &&
                p->ep->pipeline) {
            p->status = USB_RET_ADD_TO_QUEUE;
            break;
        }
        usbredir_handle_bulk_data(dev, p, ep);
        break;
    case USB_ENDPOINT_XFER_INT:
        if (ep & USB_DIR_IN) {
            usbredir_handle_interrupt_in_data(dev, p, ep);
        } else {
            usbredir_handle_interrupt_out_data(dev, p, ep);
        }
        break;
    default:
        ERROR("handle_data ep %02X has unknown type %d\n", ep,
              dev->endpoint[EP2I(ep)].type);
        p->status = USB_RET_NAK;
    }
}
