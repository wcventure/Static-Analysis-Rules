static int ohci_service_td(OHCIState *ohci, struct ohci_ed *ed)
{
    int dir;
    size_t len = 0, pktlen = 0;
#ifdef DEBUG_PACKET
    const char *str = NULL;
#endif
    int pid;
    int ret;
    int i;
    USBDevice *dev;
    struct ohci_td td;
    uint32_t addr;
    int flag_r;
    int completion;

    addr = ed->head & OHCI_DPTR_MASK;
    /
    completion = (addr == ohci->async_td);
    if (completion && !ohci->async_complete) {
#ifdef DEBUG_PACKET
        DPRINTF("Skipping async TD\n");
#endif
        return 1;
    }
    if (!ohci_read_td(ohci, addr, &td)) {
        fprintf(stderr, "usb-ohci: TD read error at %x\n", addr);
        return 0;
    }

    dir = OHCI_BM(ed->flags, ED_D);
    switch (dir) {
    case OHCI_TD_DIR_OUT:
    case OHCI_TD_DIR_IN:
        /
        break;
    default:
        dir = OHCI_BM(td.flags, TD_DP);
        break;
    }

    switch (dir) {
    case OHCI_TD_DIR_IN:
#ifdef DEBUG_PACKET
        str = "in";
#endif
        pid = USB_TOKEN_IN;
        break;
    case OHCI_TD_DIR_OUT:
#ifdef DEBUG_PACKET
        str = "out";
#endif
        pid = USB_TOKEN_OUT;
        break;
    case OHCI_TD_DIR_SETUP:
#ifdef DEBUG_PACKET
        str = "setup";
#endif
        pid = USB_TOKEN_SETUP;
        break;
    default:
        fprintf(stderr, "usb-ohci: Bad direction\n");
        return 1;
    }
    if (td.cbp && td.be) {
        if ((td.cbp & 0xfffff000) != (td.be & 0xfffff000)) {
            len = (td.be & 0xfff) + 0x1001 - (td.cbp & 0xfff);
        } else {
            len = (td.be - td.cbp) + 1;
        }

        pktlen = len;
        if (len && dir != OHCI_TD_DIR_IN) {
            /
            pktlen = (ed->flags & OHCI_ED_MPS_MASK) >> OHCI_ED_MPS_SHIFT;
            if (pktlen > len) {
                pktlen = len;
            }
            if (!completion) {
                ohci_copy_td(ohci, &td, ohci->usb_buf, pktlen, 0);
            }
        }
    }

    flag_r = (td.flags & OHCI_TD_R) != 0;
#ifdef DEBUG_PACKET
    DPRINTF(" TD @ 0x%.8x %" PRId64 " of %" PRId64
            " bytes %s r=%d cbp=0x%.8x be=0x%.8x\n",
            addr, (int64_t)pktlen, (int64_t)len, str, flag_r, td.cbp, td.be);

    if (pktlen > 0 && dir != OHCI_TD_DIR_IN) {
        DPRINTF("  data:");
        for (i = 0; i < pktlen; i++) {
            printf(" %.2x", ohci->usb_buf[i]);
        }
        DPRINTF("\n");
    }
#endif
    if (completion) {
        ret = ohci->usb_packet.result;
        ohci->async_td = 0;
        ohci->async_complete = 0;
    } else {
        ret = USB_RET_NODEV;
        for (i = 0; i < ohci->num_ports; i++) {
            dev = ohci->rhport[i].port.dev;
            if ((ohci->rhport[i].ctrl & OHCI_PORT_PES) == 0)
                continue;

            if (ohci->async_td) {
                /
#ifdef DEBUG_PACKET
                DPRINTF("Too many pending packets\n");
#endif
                return 1;
            }
            usb_packet_setup(&ohci->usb_packet, pid,
                             OHCI_BM(ed->flags, ED_FA),
                             OHCI_BM(ed->flags, ED_EN));
            usb_packet_addbuf(&ohci->usb_packet, ohci->usb_buf, pktlen);
            ret = usb_handle_packet(dev, &ohci->usb_packet);
            if (ret != USB_RET_NODEV)
                break;
        }
#ifdef DEBUG_PACKET
        DPRINTF("ret=%d\n", ret);
#endif
        if (ret == USB_RET_ASYNC) {
            ohci->async_td = addr;
            return 1;
        }
    }
    if (ret >= 0) {
        if (dir == OHCI_TD_DIR_IN) {
            ohci_copy_td(ohci, &td, ohci->usb_buf, ret, 1);
#ifdef DEBUG_PACKET
            DPRINTF("  data:");
            for (i = 0; i < ret; i++)
                printf(" %.2x", ohci->usb_buf[i]);
            DPRINTF("\n");
#endif
        } else {
            ret = pktlen;
        }
    }

    /
    if (ret == pktlen || (dir == OHCI_TD_DIR_IN && ret >= 0 && flag_r)) {
        /
        if (ret == len) {
            td.cbp = 0;
        } else {
            td.cbp += ret;
            if ((td.cbp & 0xfff) + ret > 0xfff) {
                td.cbp &= 0xfff;
                td.cbp |= td.be & ~0xfff;
            }
        }
        td.flags |= OHCI_TD_T1;
        td.flags ^= OHCI_TD_T0;
        OHCI_SET_BM(td.flags, TD_CC, OHCI_CC_NOERROR);
        OHCI_SET_BM(td.flags, TD_EC, 0);

        if ((dir != OHCI_TD_DIR_IN) && (ret != len)) {
            /
            goto exit_no_retire;
        }

        /
        ed->head &= ~OHCI_ED_C;
        if (td.flags & OHCI_TD_T0)
            ed->head |= OHCI_ED_C;
    } else {
        if (ret >= 0) {
            DPRINTF("usb-ohci: Underrun\n");
            OHCI_SET_BM(td.flags, TD_CC, OHCI_CC_DATAUNDERRUN);
        } else {
            switch (ret) {
            case USB_RET_NODEV:
                OHCI_SET_BM(td.flags, TD_CC, OHCI_CC_DEVICENOTRESPONDING);
            case USB_RET_NAK:
                DPRINTF("usb-ohci: got NAK\n");
                return 1;
            case USB_RET_STALL:
                DPRINTF("usb-ohci: got STALL\n");
                OHCI_SET_BM(td.flags, TD_CC, OHCI_CC_STALL);
                break;
            case USB_RET_BABBLE:
                DPRINTF("usb-ohci: got BABBLE\n");
                OHCI_SET_BM(td.flags, TD_CC, OHCI_CC_DATAOVERRUN);
                break;
            default:
                fprintf(stderr, "usb-ohci: Bad device response %d\n", ret);
                OHCI_SET_BM(td.flags, TD_CC, OHCI_CC_UNDEXPETEDPID);
                OHCI_SET_BM(td.flags, TD_EC, 3);
                break;
            }
        }
        ed->head |= OHCI_ED_H;
    }

    /
    ed->head &= ~OHCI_DPTR_MASK;
    ed->head |= td.next & OHCI_DPTR_MASK;
    td.next = ohci->done;
    ohci->done = addr;
    i = OHCI_BM(td.flags, TD_DI);
    if (i < ohci->done_count)
        ohci->done_count = i;
exit_no_retire:
    ohci_put_td(ohci, addr, &td);
    return OHCI_BM(td.flags, TD_CC) != OHCI_CC_NOERROR;
}
