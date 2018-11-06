static uint32_t rtl8139_TxStatus_read(RTL8139State *s, uint8_t addr, int size)
{
    uint32_t reg = (addr - TxStatus0) / 4;
    uint32_t offset = addr & 0x3;
    uint32_t ret = 0;

    if (addr & (size - 1)) {
        DPRINTF("not implemented read for TxStatus addr=0x%x size=0x%x\n", addr,
                size);
        return ret;
    }

    switch (size) {
    case 1: /
    case 2: /
    case 4:
        ret = (s->TxStatus[reg] >> offset * 8) & ((1 << (size * 8)) - 1);
        DPRINTF("TxStatus[%d] read addr=0x%x size=0x%x val=0x%08x\n", reg, addr,
                size, ret);
        break;
    default:
        DPRINTF("unsupported size 0x%x of TxStatus reading\n", size);
        break;
    }

    return ret;
}
