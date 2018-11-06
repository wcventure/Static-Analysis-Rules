static ssize_t gem_receive(VLANClientState *nc, const uint8_t *buf, size_t size)
{
    unsigned    desc[2];
    target_phys_addr_t packet_desc_addr, last_desc_addr;
    GemState *s;
    unsigned   rxbufsize, bytes_to_copy;
    unsigned   rxbuf_offset;
    uint8_t    rxbuf[2048];
    uint8_t   *rxbuf_ptr;

    s = DO_UPCAST(NICState, nc, nc)->opaque;

    /
    if (!(s->regs[GEM_NWCTRL] & GEM_NWCTRL_RXENA)) {
        return -1;
    }

    /
    if (gem_mac_address_filter(s, buf) == GEM_RX_REJECT) {
        return -1;
    }

    /
    if (s->regs[GEM_NWCFG] & GEM_NWCFG_LERR_DISC) {
        unsigned type_len;

        /
        type_len = buf[12] << 8 | buf[13];
        /
        if (type_len < 0x600) {
            if (size < type_len) {
                /
                return -1;
            }
        }
    }

    /
    rxbuf_offset = (s->regs[GEM_NWCFG] & GEM_NWCFG_BUFF_OFST_M) >>
                   GEM_NWCFG_BUFF_OFST_S;

    /
    rxbufsize = ((s->regs[GEM_DMACFG] & GEM_DMACFG_RBUFSZ_M) >>
                 GEM_DMACFG_RBUFSZ_S) * GEM_DMACFG_RBUFSZ_MUL;
    bytes_to_copy = size;

    /
    if (s->regs[GEM_NWCFG] & GEM_NWCFG_STRIP_FCS) {
        rxbuf_ptr = (void *)buf;
    } else {
        unsigned crc_val;
        int      crc_offset;

        /

        memcpy(rxbuf, buf, size);
        memset(rxbuf + size, 0, sizeof(rxbuf - size));
        rxbuf_ptr = rxbuf;
        crc_val = cpu_to_le32(crc32(0, rxbuf, MAX(size, 60)));
        if (size < 60) {
            crc_offset = 60;
        } else {
            crc_offset = size;
        }
        memcpy(rxbuf + crc_offset, &crc_val, sizeof(crc_val));

        bytes_to_copy += 4;
        size += 4;
    }

    /
    if (size < 64) {
        size = 64;
    }

    DB_PRINT("config bufsize: %d packet size: %ld\n", rxbufsize, size);

    packet_desc_addr = s->rx_desc_addr;
    while (1) {
        DB_PRINT("read descriptor 0x%x\n", packet_desc_addr);
        /
        cpu_physical_memory_read(packet_desc_addr,
                                 (uint8_t *)&desc[0], sizeof(desc));

        /
        if (rx_desc_get_ownership(desc) == 1) {
            DB_PRINT("descriptor 0x%x owned by sw.\n", packet_desc_addr);
            s->regs[GEM_RXSTATUS] |= GEM_RXSTATUS_NOBUF;
            /
            gem_update_int_status(s);
            return -1;
        }

        DB_PRINT("copy %d bytes to 0x%x\n", MIN(bytes_to_copy, rxbufsize),
                rx_desc_get_buffer(desc));

        /
        if (rx_desc_get_buffer(desc) == 0) {
            DB_PRINT("Invalid RX buffer (NULL) for descriptor 0x%x\n",
                       packet_desc_addr);
            break;
        }

        /
        cpu_physical_memory_write(rx_desc_get_buffer(desc) + rxbuf_offset,
                                  rxbuf_ptr, MIN(bytes_to_copy, rxbufsize));
        bytes_to_copy -= MIN(bytes_to_copy, rxbufsize);
        rxbuf_ptr += MIN(bytes_to_copy, rxbufsize);
        if (bytes_to_copy == 0) {
            break;
        }

        /
        if (rx_desc_get_wrap(desc)) {
            packet_desc_addr = s->regs[GEM_RXQBASE];
        } else {
            packet_desc_addr += 8;
        }
    }

    DB_PRINT("set length: %ld, EOF on descriptor 0x%x\n", size,
            (unsigned)packet_desc_addr);

    /
    rx_desc_set_eof(desc);
    rx_desc_set_length(desc, size);
    cpu_physical_memory_write(packet_desc_addr,
                              (uint8_t *)&desc[0], sizeof(desc));

    /
    last_desc_addr = packet_desc_addr;
    packet_desc_addr = s->rx_desc_addr;
    s->rx_desc_addr = last_desc_addr;
    if (rx_desc_get_wrap(desc)) {
        s->rx_desc_addr = s->regs[GEM_RXQBASE];
    } else {
        s->rx_desc_addr += 8;
    }

    DB_PRINT("set SOF, OWN on descriptor 0x%08x\n", packet_desc_addr);

    /
    gem_receive_updatestats(s, buf, size);

    /
    /
    cpu_physical_memory_read(packet_desc_addr,
                             (uint8_t *)&desc[0], sizeof(desc));
    rx_desc_set_sof(desc);
    rx_desc_set_ownership(desc);
    cpu_physical_memory_write(packet_desc_addr,
                              (uint8_t *)&desc[0], sizeof(desc));

    s->regs[GEM_RXSTATUS] |= GEM_RXSTATUS_FRMRCVD;

    /
    gem_update_int_status(s);

    return size;
}
