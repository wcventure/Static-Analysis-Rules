static void m5206_mbar_writew(void *opaque, target_phys_addr_t offset,
                              uint32_t value)
{
    m5206_mbar_state *s = (m5206_mbar_state *)opaque;
    int width;
    offset &= 0x3ff;
    if (offset > 0x200) {
        hw_error("Bad MBAR write offset 0x%x", (int)offset);
    }
    width = m5206_mbar_width[offset >> 2];
    if (width > 2) {
        uint32_t tmp;
        tmp = m5206_mbar_readl(opaque, offset & ~3);
        if (offset & 3) {
            tmp = (tmp & 0xffff0000) | value;
        } else {
            tmp = (tmp & 0x0000ffff) | (value << 16);
        }
        m5206_mbar_writel(opaque, offset & ~3, tmp);
        return;
    } else if (width < 2) {
        m5206_mbar_writeb(opaque, offset, value >> 8);
        m5206_mbar_writeb(opaque, offset + 1, value & 0xff);
        return;
    }
    m5206_mbar_write(s, offset, value, 2);
}
