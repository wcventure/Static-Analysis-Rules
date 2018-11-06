static uint64_t mv88w8618_eth_read(void *opaque, hwaddr offset,
                                   unsigned size)
{
    mv88w8618_eth_state *s = opaque;

    switch (offset) {
    case MP_ETH_SMIR:
        if (s->smir & MP_ETH_SMIR_OPCODE) {
            switch (s->smir & MP_ETH_SMIR_ADDR) {
            case MP_ETH_PHY1_BMSR:
                return MP_PHY_BMSR_LINK | MP_PHY_BMSR_AUTONEG |
                       MP_ETH_SMIR_RDVALID;
            case MP_ETH_PHY1_PHYSID1:
                return (MP_PHY_88E3015 >> 16) | MP_ETH_SMIR_RDVALID;
            case MP_ETH_PHY1_PHYSID2:
                return (MP_PHY_88E3015 & 0xFFFF) | MP_ETH_SMIR_RDVALID;
            default:
                return MP_ETH_SMIR_RDVALID;
            }
        }
        return 0;

    case MP_ETH_ICR:
        return s->icr;

    case MP_ETH_IMR:
        return s->imr;

    case MP_ETH_FRDP0 ... MP_ETH_FRDP3:
        return s->frx_queue[(offset - MP_ETH_FRDP0)/4];

    case MP_ETH_CRDP0 ... MP_ETH_CRDP3:
        return s->rx_queue[(offset - MP_ETH_CRDP0)/4];

    case MP_ETH_CTDP0 ... MP_ETH_CTDP3:
        return s->tx_queue[(offset - MP_ETH_CTDP0)/4];

    default:
        return 0;
    }
}
