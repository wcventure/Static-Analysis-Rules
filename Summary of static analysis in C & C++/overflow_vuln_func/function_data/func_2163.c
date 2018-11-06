static void eepro100_cu_command(EEPRO100State * s, uint8_t val)
{
    eepro100_tx_t tx;
    uint32_t cb_address;
    switch (val) {
    case CU_NOP:
        /
        break;
    case CU_START:
        if (get_cu_state(s) != cu_idle) {
            /
            logout("CU state is %u, should be %u\n", get_cu_state(s), cu_idle);
            //~ assert(!"wrong CU state");
        }
        set_cu_state(s, cu_active);
        s->cu_offset = s->pointer;
      next_command:
        cb_address = s->cu_base + s->cu_offset;
        cpu_physical_memory_read(cb_address, (uint8_t *) & tx, sizeof(tx));
        uint16_t status = le16_to_cpu(tx.status);
        uint16_t command = le16_to_cpu(tx.command);
        logout
            ("val=0x%02x (cu start), status=0x%04x, command=0x%04x, link=0x%08x\n",
             val, status, command, tx.link);
        bool bit_el = ((command & 0x8000) != 0);
        bool bit_s = ((command & 0x4000) != 0);
        bool bit_i = ((command & 0x2000) != 0);
        bool bit_nc = ((command & 0x0010) != 0);
        //~ bool bit_sf = ((command & 0x0008) != 0);
        uint16_t cmd = command & 0x0007;
        s->cu_offset = le32_to_cpu(tx.link);
        switch (cmd) {
        case CmdNOp:
            /
            break;
        case CmdIASetup:
            cpu_physical_memory_read(cb_address + 8, &s->macaddr[0], 6);
            logout("macaddr: %s\n", nic_dump(&s->macaddr[0], 6));
            break;
        case CmdConfigure:
            cpu_physical_memory_read(cb_address + 8, &s->configuration[0],
                                     sizeof(s->configuration));
            logout("configuration: %s\n", nic_dump(&s->configuration[0], 16));
            break;
        case CmdMulticastList:
            //~ missing("multicast list");
            break;
        case CmdTx:
            (void)0;
            uint32_t tbd_array = le32_to_cpu(tx.tx_desc_addr);
            uint16_t tcb_bytes = (le16_to_cpu(tx.tcb_bytes) & 0x3fff);
            logout
                ("transmit, TBD array address 0x%08x, TCB byte count 0x%04x, TBD count %u\n",
                 tbd_array, tcb_bytes, tx.tbd_count);
            assert(!bit_nc);
            //~ assert(!bit_sf);
            assert(tcb_bytes <= 2600);
            /
            //~ assert((tcb_bytes > 0) || (tbd_array != 0xffffffff));
            if (!((tcb_bytes > 0) || (tbd_array != 0xffffffff))) {
                logout
                    ("illegal values of TBD array address and TCB byte count!\n");
            }
            uint8_t buf[MAX_ETH_FRAME_SIZE + 4];
            uint16_t size = 0;
            uint32_t tbd_address = cb_address + 0x10;
            assert(tcb_bytes <= sizeof(buf));
            while (size < tcb_bytes) {
                uint32_t tx_buffer_address = ldl_phys(tbd_address);
                uint16_t tx_buffer_size = lduw_phys(tbd_address + 4);
                //~ uint16_t tx_buffer_el = lduw_phys(tbd_address + 6);
                tbd_address += 8;
                logout
                    ("TBD (simplified mode): buffer address 0x%08x, size 0x%04x\n",
                     tx_buffer_address, tx_buffer_size);
                cpu_physical_memory_read(tx_buffer_address, &buf[size],
                                         tx_buffer_size);
                size += tx_buffer_size;
            }
            if (tbd_array == 0xffffffff) {
                /
            } else {
                /
                uint8_t tbd_count = 0;
                if (!(s->configuration[6] & BIT(4))) {
                    /
                    assert(tcb_bytes == 0);
                    for (; tbd_count < 2; tbd_count++) {
                        uint32_t tx_buffer_address = ldl_phys(tbd_address);
                        uint16_t tx_buffer_size = lduw_phys(tbd_address + 4);
                        uint16_t tx_buffer_el = lduw_phys(tbd_address + 6);
                        tbd_address += 8;
                        logout
                            ("TBD (extended mode): buffer address 0x%08x, size 0x%04x\n",
                             tx_buffer_address, tx_buffer_size);
                        cpu_physical_memory_read(tx_buffer_address, &buf[size],
                                                 tx_buffer_size);
                        size += tx_buffer_size;
                        if (tx_buffer_el & 1) {
                            break;
                        }
                    }
                }
                tbd_address = tbd_array;
                for (; tbd_count < tx.tbd_count; tbd_count++) {
                    uint32_t tx_buffer_address = ldl_phys(tbd_address);
                    uint16_t tx_buffer_size = lduw_phys(tbd_address + 4);
                    uint16_t tx_buffer_el = lduw_phys(tbd_address + 6);
                    tbd_address += 8;
                    logout
                        ("TBD (flexible mode): buffer address 0x%08x, size 0x%04x\n",
                         tx_buffer_address, tx_buffer_size);
                    cpu_physical_memory_read(tx_buffer_address, &buf[size],
                                             tx_buffer_size);
                    size += tx_buffer_size;
                    if (tx_buffer_el & 1) {
                        break;
                    }
                }
            }
            qemu_send_packet(s->vc, buf, size);
            s->statistics.tx_good_frames++;
            /
            //~ eepro100_cx_interrupt(s);
            break;
        case CmdTDR:
            logout("load microcode\n");
            /
            break;
        default:
            missing("undefined command");
        }
        /
        stw_phys(cb_address, status | 0x8000 | 0x2000);
        if (bit_i) {
            /
            eepro100_cx_interrupt(s);
        }
        if (bit_el) {
            /
            set_cu_state(s, cu_idle);
            eepro100_cna_interrupt(s);
        } else if (bit_s) {
            /
            set_cu_state(s, cu_suspended);
            eepro100_cna_interrupt(s);
        } else {
            /
            logout("CU list with at least one more entry\n");
            goto next_command;
        }
        logout("CU list empty\n");
        /
        break;
    case CU_RESUME:
        if (get_cu_state(s) != cu_suspended) {
            logout("bad CU resume from CU state %u\n", get_cu_state(s));
            /
            //~ missing("cu resume");
            set_cu_state(s, cu_suspended);
        }
        if (get_cu_state(s) == cu_suspended) {
            logout("CU resuming\n");
            set_cu_state(s, cu_active);
            goto next_command;
        }
        break;
    case CU_STATSADDR:
        /
        s->statsaddr = s->pointer;
        logout("val=0x%02x (status address)\n", val);
        break;
    case CU_SHOWSTATS:
        /
        dump_statistics(s);
        break;
    case CU_CMD_BASE:
        /
        logout("val=0x%02x (CU base address)\n", val);
        s->cu_base = s->pointer;
        break;
    case CU_DUMPSTATS:
        /
        dump_statistics(s);
        memset(&s->statistics, 0, sizeof(s->statistics));
        break;
    case CU_SRESUME:
        /
        missing("CU static resume");
        break;
    default:
        missing("Undefined CU command");
    }
}
