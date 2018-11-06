static void mv88w8618_eth_write(void *opaque, hwaddr offset,
                                uint64_t value, unsigned size)
{
    mv88w8618_eth_state *s = opaque;

    switch (offset) {
    case MP_ETH_SMIR:
        s->smir = value;
        break;

    case MP_ETH_PCXR:
        s->vlan_header = ((value >> MP_ETH_PCXR_2BSM_BIT) & 1) * 2;
        break;

    case MP_ETH_SDCMR:
        if (value & MP_ETH_CMD_TXHI) {
            eth_send(s, 1);
        }
        if (value & MP_ETH_CMD_TXLO) {
            eth_send(s, 0);
        }
        if (value & (MP_ETH_CMD_TXHI | MP_ETH_CMD_TXLO) && s->icr & s->imr) {
            qemu_irq_raise(s->irq);
        }
        break;

    case MP_ETH_ICR:
        s->icr &= value;
        break;

    case MP_ETH_IMR:
        s->imr = value;
        if (s->icr & s->imr) {
            qemu_irq_raise(s->irq);
        }
        break;

    case MP_ETH_FRDP0 ... MP_ETH_FRDP3:
        s->frx_queue[(offset - MP_ETH_FRDP0)/4] = value;
        break;

    case MP_ETH_CRDP0 ... MP_ETH_CRDP3:
        s->rx_queue[(offset - MP_ETH_CRDP0)/4] =
            s->cur_rx[(offset - MP_ETH_CRDP0)/4] = value;
        break;

    case MP_ETH_CTDP0 ... MP_ETH_CTDP3:
        s->tx_queue[(offset - MP_ETH_CTDP0)/4] = value;
        break;
    }
}
