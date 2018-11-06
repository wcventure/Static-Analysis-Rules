static void usb_mtp_handle_data(USBDevice *dev, USBPacket *p)
{
    MTPState *s = USB_MTP(dev);
    MTPControl cmd;
    mtp_container container;
    uint32_t params[5];
    int i, rc;

    switch (p->ep->nr) {
    case EP_DATA_IN:
        if (s->data_out != NULL) {
            /
            trace_usb_mtp_stall(s->dev.addr, "awaiting data-out");
            p->status = USB_RET_STALL;
            return;
        }
        if (p->iov.size < sizeof(container)) {
            trace_usb_mtp_stall(s->dev.addr, "packet too small");
            p->status = USB_RET_STALL;
            return;
        }
        if (s->data_in !=  NULL) {
            MTPData *d = s->data_in;
            int dlen = d->length - d->offset;
            if (d->first) {
                trace_usb_mtp_data_in(s->dev.addr, d->trans, d->length);
                container.length = cpu_to_le32(d->length + sizeof(container));
                container.type   = cpu_to_le16(TYPE_DATA);
                container.code   = cpu_to_le16(d->code);
                container.trans  = cpu_to_le32(d->trans);
                usb_packet_copy(p, &container, sizeof(container));
                d->first = false;
                if (dlen > p->iov.size - sizeof(container)) {
                    dlen = p->iov.size - sizeof(container);
                }
            } else {
                if (dlen > p->iov.size) {
                    dlen = p->iov.size;
                }
            }
            if (d->fd == -1) {
                usb_packet_copy(p, d->data + d->offset, dlen);
            } else {
                if (d->alloc < p->iov.size) {
                    d->alloc = p->iov.size;
                    d->data = g_realloc(d->data, d->alloc);
                }
                rc = read(d->fd, d->data, dlen);
                if (rc != dlen) {
                    memset(d->data, 0, dlen);
                    s->result->code = RES_INCOMPLETE_TRANSFER;
                }
                usb_packet_copy(p, d->data, dlen);
            }
            d->offset += dlen;
            if (d->offset == d->length) {
                usb_mtp_data_free(s->data_in);
                s->data_in = NULL;
            }
        } else if (s->result != NULL) {
            MTPControl *r = s->result;
            int length = sizeof(container) + r->argc * sizeof(uint32_t);
            if (r->code == RES_OK) {
                trace_usb_mtp_success(s->dev.addr, r->trans,
                                      (r->argc > 0) ? r->argv[0] : 0,
                                      (r->argc > 1) ? r->argv[1] : 0);
            } else {
                trace_usb_mtp_error(s->dev.addr, r->code, r->trans,
                                    (r->argc > 0) ? r->argv[0] : 0,
                                    (r->argc > 1) ? r->argv[1] : 0);
            }
            container.length = cpu_to_le32(length);
            container.type   = cpu_to_le16(TYPE_RESPONSE);
            container.code   = cpu_to_le16(r->code);
            container.trans  = cpu_to_le32(r->trans);
            for (i = 0; i < r->argc; i++) {
                params[i] = cpu_to_le32(r->argv[i]);
            }
            usb_packet_copy(p, &container, sizeof(container));
            usb_packet_copy(p, &params, length - sizeof(container));
            g_free(s->result);
            s->result = NULL;
        }
        break;
    case EP_DATA_OUT:
        if (p->iov.size < sizeof(container)) {
            trace_usb_mtp_stall(s->dev.addr, "packet too small");
            p->status = USB_RET_STALL;
            return;
        }
        usb_packet_copy(p, &container, sizeof(container));
        switch (le16_to_cpu(container.type)) {
        case TYPE_COMMAND:
            if (s->data_in || s->data_out || s->result) {
                trace_usb_mtp_stall(s->dev.addr, "transaction inflight");
                p->status = USB_RET_STALL;
                return;
            }
            cmd.code = le16_to_cpu(container.code);
            cmd.argc = (le32_to_cpu(container.length) - sizeof(container))
                / sizeof(uint32_t);
            cmd.trans = le32_to_cpu(container.trans);
            if (cmd.argc > ARRAY_SIZE(cmd.argv)) {
                cmd.argc = ARRAY_SIZE(cmd.argv);
            }
            if (p->iov.size < sizeof(container) + cmd.argc * sizeof(uint32_t)) {
                trace_usb_mtp_stall(s->dev.addr, "packet too small");
                p->status = USB_RET_STALL;
                return;
            }
            usb_packet_copy(p, &params, cmd.argc * sizeof(uint32_t));
            for (i = 0; i < cmd.argc; i++) {
                cmd.argv[i] = le32_to_cpu(params[i]);
            }
            trace_usb_mtp_command(s->dev.addr, cmd.code, cmd.trans,
                                  (cmd.argc > 0) ? cmd.argv[0] : 0,
                                  (cmd.argc > 1) ? cmd.argv[1] : 0,
                                  (cmd.argc > 2) ? cmd.argv[2] : 0,
                                  (cmd.argc > 3) ? cmd.argv[3] : 0,
                                  (cmd.argc > 4) ? cmd.argv[4] : 0);
            usb_mtp_command(s, &cmd);
            break;
        default:
            /
            p->status = USB_RET_STALL;
            return;
        }
        break;
    case EP_EVENT:
#ifdef CONFIG_INOTIFY1
        if (!QTAILQ_EMPTY(&s->events)) {
            struct MTPMonEntry *e = QTAILQ_LAST(&s->events, events);
            uint32_t handle;
            int len = sizeof(container) + sizeof(uint32_t);

            if (p->iov.size < len) {
                trace_usb_mtp_stall(s->dev.addr,
                                    "packet too small to send event");
                p->status = USB_RET_STALL;
                return;
            }

            QTAILQ_REMOVE(&s->events, e, next);
            container.length = cpu_to_le32(len);
            container.type = cpu_to_le32(TYPE_EVENT);
            container.code = cpu_to_le16(e->event);
            container.trans = 0; /
            handle = cpu_to_le32(e->handle);
            usb_packet_copy(p, &container, sizeof(container));
            usb_packet_copy(p, &handle, sizeof(uint32_t));
            g_free(e);
            return;
        }
#endif
        p->status = USB_RET_NAK;
        return;
    default:
        trace_usb_mtp_stall(s->dev.addr, "invalid endpoint");
        p->status = USB_RET_STALL;
        return;
    }

    if (p->actual_length == 0) {
        trace_usb_mtp_nak(s->dev.addr, p->ep->nr);
        p->status = USB_RET_NAK;
        return;
    } else {
        trace_usb_mtp_xfer(s->dev.addr, p->ep->nr, p->actual_length,
                           p->iov.size);
        return;
    }
}
