static ssize_t rtl8139_do_receive(NetClientState *nc, const uint8_t *buf, size_t size_, int do_interrupt)
{
    RTL8139State *s = qemu_get_nic_opaque(nc);
    PCIDevice *d = PCI_DEVICE(s);
    /
    int size = size_;
    const uint8_t *dot1q_buf = NULL;

    uint32_t packet_header = 0;

    uint8_t buf1[MIN_BUF_SIZE + VLAN_HLEN];
    static const uint8_t broadcast_macaddr[6] =
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

    DPRINTF(">>> received len=%d\n", size);

    /
    if (!s->clock_enabled)
    {
        DPRINTF("stopped ==========================\n");
        return -1;
    }

    /

    if (!rtl8139_receiver_enabled(s))
    {
        DPRINTF("receiver disabled ================\n");
        return -1;
    }

    /
    if (s->RxConfig & AcceptAllPhys) {
        /
        DPRINTF(">>> packet received in promiscuous mode\n");

    } else {
        if (!memcmp(buf,  broadcast_macaddr, 6)) {
            /
            if (!(s->RxConfig & AcceptBroadcast))
            {
                DPRINTF(">>> broadcast packet rejected\n");

                /
                ++s->tally_counters.RxERR;

                return size;
            }

            packet_header |= RxBroadcast;

            DPRINTF(">>> broadcast packet received\n");

            /
            ++s->tally_counters.RxOkBrd;

        } else if (buf[0] & 0x01) {
            /
            if (!(s->RxConfig & AcceptMulticast))
            {
                DPRINTF(">>> multicast packet rejected\n");

                /
                ++s->tally_counters.RxERR;

                return size;
            }

            int mcast_idx = compute_mcast_idx(buf);

            if (!(s->mult[mcast_idx >> 3] & (1 << (mcast_idx & 7))))
            {
                DPRINTF(">>> multicast address mismatch\n");

                /
                ++s->tally_counters.RxERR;

                return size;
            }

            packet_header |= RxMulticast;

            DPRINTF(">>> multicast packet received\n");

            /
            ++s->tally_counters.RxOkMul;

        } else if (s->phys[0] == buf[0] &&
                   s->phys[1] == buf[1] &&
                   s->phys[2] == buf[2] &&
                   s->phys[3] == buf[3] &&
                   s->phys[4] == buf[4] &&
                   s->phys[5] == buf[5]) {
            /
            if (!(s->RxConfig & AcceptMyPhys))
            {
                DPRINTF(">>> rejecting physical address matching packet\n");

                /
                ++s->tally_counters.RxERR;

                return size;
            }

            packet_header |= RxPhysical;

            DPRINTF(">>> physical address matching packet received\n");

            /
            ++s->tally_counters.RxOkPhy;

        } else {

            DPRINTF(">>> unknown packet\n");

            /
            ++s->tally_counters.RxERR;

            return size;
        }
    }

    /
    if (size < MIN_BUF_SIZE + VLAN_HLEN) {
        memcpy(buf1, buf, size);
        memset(buf1 + size, 0, MIN_BUF_SIZE + VLAN_HLEN - size);
        buf = buf1;
        if (size < MIN_BUF_SIZE) {
            size = MIN_BUF_SIZE;
        }
    }

    if (rtl8139_cp_receiver_enabled(s))
    {
        if (!rtl8139_cp_rx_valid(s)) {
            return size;
        }

        DPRINTF("in C+ Rx mode ================\n");

        /

/
#define CP_RX_OWN (1<<31)
/
#define CP_RX_EOR (1<<30)
/
#define CP_RX_BUFFER_SIZE_MASK ((1<<13) - 1)
/
#define CP_RX_TAVA (1<<16)
/
#define CP_RX_VLAN_TAG_MASK ((1<<16) - 1)
/
/

        int descriptor = s->currCPlusRxDesc;
        dma_addr_t cplus_rx_ring_desc;

        cplus_rx_ring_desc = rtl8139_addr64(s->RxRingAddrLO, s->RxRingAddrHI);
        cplus_rx_ring_desc += 16 * descriptor;

        DPRINTF("+++ C+ mode reading RX descriptor %d from host memory at "
            "%08x %08x = "DMA_ADDR_FMT"\n", descriptor, s->RxRingAddrHI,
            s->RxRingAddrLO, cplus_rx_ring_desc);

        uint32_t val, rxdw0,rxdw1,rxbufLO,rxbufHI;

        pci_dma_read(d, cplus_rx_ring_desc, &val, 4);
        rxdw0 = le32_to_cpu(val);
        pci_dma_read(d, cplus_rx_ring_desc+4, &val, 4);
        rxdw1 = le32_to_cpu(val);
        pci_dma_read(d, cplus_rx_ring_desc+8, &val, 4);
        rxbufLO = le32_to_cpu(val);
        pci_dma_read(d, cplus_rx_ring_desc+12, &val, 4);
        rxbufHI = le32_to_cpu(val);

        DPRINTF("+++ C+ mode RX descriptor %d %08x %08x %08x %08x\n",
            descriptor, rxdw0, rxdw1, rxbufLO, rxbufHI);

        if (!(rxdw0 & CP_RX_OWN))
        {
            DPRINTF("C+ Rx mode : descriptor %d is owned by host\n",
                descriptor);

            s->IntrStatus |= RxOverflow;
            ++s->RxMissed;

            /
            ++s->tally_counters.RxERR;
            ++s->tally_counters.MissPkt;

            rtl8139_update_irq(s);
            return size_;
        }

        uint32_t rx_space = rxdw0 & CP_RX_BUFFER_SIZE_MASK;

        /
        if (s->CpCmd & CPlusRxVLAN && be16_to_cpup((uint16_t *)
                &buf[ETH_ALEN * 2]) == ETH_P_VLAN) {
            dot1q_buf = &buf[ETH_ALEN * 2];
            size -= VLAN_HLEN;
            /
            if (size < MIN_BUF_SIZE) {
                size = MIN_BUF_SIZE;
            }

            rxdw1 &= ~CP_RX_VLAN_TAG_MASK;
            /
            rxdw1 |= CP_RX_TAVA | le16_to_cpup((uint16_t *)
                &dot1q_buf[ETHER_TYPE_LEN]);

            DPRINTF("C+ Rx mode : extracted vlan tag with tci: ""%u\n",
                be16_to_cpup((uint16_t *)&dot1q_buf[ETHER_TYPE_LEN]));
        } else {
            /
            rxdw1 &= ~CP_RX_TAVA;
        }

        /

        if (size+4 > rx_space)
        {
            DPRINTF("C+ Rx mode : descriptor %d size %d received %d + 4\n",
                descriptor, rx_space, size);

            s->IntrStatus |= RxOverflow;
            ++s->RxMissed;

            /
            ++s->tally_counters.RxERR;
            ++s->tally_counters.MissPkt;

            rtl8139_update_irq(s);
            return size_;
        }

        dma_addr_t rx_addr = rtl8139_addr64(rxbufLO, rxbufHI);

        /
        if (dot1q_buf) {
            pci_dma_write(d, rx_addr, buf, 2 * ETH_ALEN);
            pci_dma_write(d, rx_addr + 2 * ETH_ALEN,
                          buf + 2 * ETH_ALEN + VLAN_HLEN,
                          size - 2 * ETH_ALEN);
        } else {
            pci_dma_write(d, rx_addr, buf, size);
        }

        if (s->CpCmd & CPlusRxChkSum)
        {
            /
        }

        /
        val = cpu_to_le32(crc32(0, buf, size_));
        pci_dma_write(d, rx_addr+size, (uint8_t *)&val, 4);

/
#define CP_RX_STATUS_FS (1<<29)
/
#define CP_RX_STATUS_LS (1<<28)
/
#define CP_RX_STATUS_MAR (1<<26)
/
#define CP_RX_STATUS_PAM (1<<25)
/
#define CP_RX_STATUS_BAR (1<<24)
/
#define CP_RX_STATUS_RUNT (1<<19)
/
#define CP_RX_STATUS_CRC (1<<18)
/
#define CP_RX_STATUS_IPF (1<<15)
/
#define CP_RX_STATUS_UDPF (1<<14)
/
#define CP_RX_STATUS_TCPF (1<<13)

        /
        rxdw0 &= ~CP_RX_OWN;

        /
        rxdw0 |= CP_RX_STATUS_FS;

        /
        rxdw0 |= CP_RX_STATUS_LS;

        /
        if (packet_header & RxBroadcast)
            rxdw0 |= CP_RX_STATUS_BAR;
        if (packet_header & RxMulticast)
            rxdw0 |= CP_RX_STATUS_MAR;
        if (packet_header & RxPhysical)
            rxdw0 |= CP_RX_STATUS_PAM;

        /
        rxdw0 &= ~CP_RX_BUFFER_SIZE_MASK;
        rxdw0 |= (size+4);

        /
        val = cpu_to_le32(rxdw0);
        pci_dma_write(d, cplus_rx_ring_desc, (uint8_t *)&val, 4);
        val = cpu_to_le32(rxdw1);
        pci_dma_write(d, cplus_rx_ring_desc+4, (uint8_t *)&val, 4);

        /
        ++s->tally_counters.RxOk;

        /
        if (rxdw0 & CP_RX_EOR)
        {
            s->currCPlusRxDesc = 0;
        }
        else
        {
            ++s->currCPlusRxDesc;
        }

        DPRINTF("done C+ Rx mode ----------------\n");

    }
    else
    {
        DPRINTF("in ring Rx mode ================\n");

        /
        int avail = MOD2(s->RxBufferSize + s->RxBufPtr - s->RxBufAddr, s->RxBufferSize);

        /

        if (avail != 0 && size + 8 >= avail)
        {
            DPRINTF("rx overflow: rx buffer length %d head 0x%04x "
                "read 0x%04x === available 0x%04x need 0x%04x\n",
                s->RxBufferSize, s->RxBufAddr, s->RxBufPtr, avail, size + 8);

            s->IntrStatus |= RxOverflow;
            ++s->RxMissed;
            rtl8139_update_irq(s);
            return size_;
        }

        packet_header |= RxStatusOK;

        packet_header |= (((size+4) << 16) & 0xffff0000);

        /
        uint32_t val = cpu_to_le32(packet_header);

        rtl8139_write_buffer(s, (uint8_t *)&val, 4);

        rtl8139_write_buffer(s, buf, size);

        /
        val = cpu_to_le32(crc32(0, buf, size));
        rtl8139_write_buffer(s, (uint8_t *)&val, 4);

        /
        s->RxBufAddr = MOD2((s->RxBufAddr + 3) & ~0x3, s->RxBufferSize);

        /

        DPRINTF("received: rx buffer length %d head 0x%04x read 0x%04x\n",
            s->RxBufferSize, s->RxBufAddr, s->RxBufPtr);
    }

    s->IntrStatus |= RxOK;

    if (do_interrupt)
    {
        rtl8139_update_irq(s);
    }

    return size_;
}
