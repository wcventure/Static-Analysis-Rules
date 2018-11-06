static void gem_write(void *opaque, hwaddr offset, uint64_t val,
        unsigned size)
{
    CadenceGEMState *s = (CadenceGEMState *)opaque;
    uint32_t readonly;
    int i;

    DB_PRINT("offset: 0x%04x write: 0x%08x ", (unsigned)offset, (unsigned)val);
    offset >>= 2;

    /
    val &= ~(s->regs_ro[offset]);
    /
    readonly = s->regs[offset] & (s->regs_ro[offset] | s->regs_w1c[offset]);

    /
    s->regs[offset] = (val & ~s->regs_w1c[offset]) | readonly;

    /
    s->regs[offset] &= ~(s->regs_w1c[offset] & val);

    /
    switch (offset) {
    case GEM_NWCTRL:
        if (val & GEM_NWCTRL_RXENA) {
            for (i = 0; i < s->num_priority_queues; ++i) {
                gem_get_rx_desc(s, i);
            }
        }
        if (val & GEM_NWCTRL_TXSTART) {
            gem_transmit(s);
        }
        if (!(val & GEM_NWCTRL_TXENA)) {
            /
            for (i = 0; i < s->num_priority_queues; i++) {
                s->tx_desc_addr[i] = s->regs[GEM_TXQBASE];
            }
        }
        if (gem_can_receive(qemu_get_queue(s->nic))) {
            qemu_flush_queued_packets(qemu_get_queue(s->nic));
        }
        break;

    case GEM_TXSTATUS:
        gem_update_int_status(s);
        break;
    case GEM_RXQBASE:
        s->rx_desc_addr[0] = val;
        break;
    case GEM_RECEIVE_Q1_PTR ... GEM_RECEIVE_Q15_PTR:
        s->rx_desc_addr[offset - GEM_RECEIVE_Q1_PTR + 1] = val;
        break;
    case GEM_TXQBASE:
        s->tx_desc_addr[0] = val;
        break;
    case GEM_TRANSMIT_Q1_PTR ... GEM_TRANSMIT_Q15_PTR:
        s->tx_desc_addr[offset - GEM_TRANSMIT_Q1_PTR + 1] = val;
        break;
    case GEM_RXSTATUS:
        gem_update_int_status(s);
        break;
    case GEM_IER:
        s->regs[GEM_IMR] &= ~val;
        gem_update_int_status(s);
        break;
    case GEM_INT_Q1_ENABLE ... GEM_INT_Q7_ENABLE:
        s->regs[GEM_INT_Q1_MASK + offset - GEM_INT_Q1_ENABLE] &= ~val;
        gem_update_int_status(s);
        break;
    case GEM_INT_Q8_ENABLE ... GEM_INT_Q15_ENABLE:
        s->regs[GEM_INT_Q8_MASK + offset - GEM_INT_Q8_ENABLE] &= ~val;
        gem_update_int_status(s);
        break;
    case GEM_IDR:
        s->regs[GEM_IMR] |= val;
        gem_update_int_status(s);
        break;
    case GEM_INT_Q1_DISABLE ... GEM_INT_Q7_DISABLE:
        s->regs[GEM_INT_Q1_MASK + offset - GEM_INT_Q1_DISABLE] |= val;
        gem_update_int_status(s);
        break;
    case GEM_INT_Q8_DISABLE ... GEM_INT_Q15_DISABLE:
        s->regs[GEM_INT_Q8_MASK + offset - GEM_INT_Q8_DISABLE] |= val;
        gem_update_int_status(s);
        break;
    case GEM_SPADDR1LO:
    case GEM_SPADDR2LO:
    case GEM_SPADDR3LO:
    case GEM_SPADDR4LO:
        s->sar_active[(offset - GEM_SPADDR1LO) / 2] = false;
        break;
    case GEM_SPADDR1HI:
    case GEM_SPADDR2HI:
    case GEM_SPADDR3HI:
    case GEM_SPADDR4HI:
        s->sar_active[(offset - GEM_SPADDR1HI) / 2] = true;
        break;
    case GEM_PHYMNTNC:
        if (val & GEM_PHYMNTNC_OP_W) {
            uint32_t phy_addr, reg_num;

            phy_addr = (val & GEM_PHYMNTNC_ADDR) >> GEM_PHYMNTNC_ADDR_SHFT;
            if (phy_addr == BOARD_PHY_ADDRESS || phy_addr == 0) {
                reg_num = (val & GEM_PHYMNTNC_REG) >> GEM_PHYMNTNC_REG_SHIFT;
                gem_phy_write(s, reg_num, val);
            }
        }
        break;
    }

    DB_PRINT("newval: 0x%08x\n", s->regs[offset]);
}
