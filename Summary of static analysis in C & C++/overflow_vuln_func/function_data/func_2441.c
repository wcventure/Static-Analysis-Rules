static void m5206_mbar_writeb(void *opaque, target_phys_addr_t offset,
                              uint32_t value)
{
    m5206_mbar_state *s = (m5206_mbar_state *)opaque;
    int width;
    offset &= 0x3ff;
    if (offset > 0x200) {
        hw_error("Bad MBAR write offset 0x%x", (int)offset);
    }
    width = m5206_mbar_width[offset >> 2];
    if (width > 1) {
        uint32_t tmp;
        tmp = m5206_mbar_readw(opaque, offset & ~1);
        if (offset & 1) {
            tmp = (tmp & 0xff00) | value;
        } else {
            tmp = (tmp & 0x00ff) | (value << 8);
        }
        m5206_mbar_writew(opaque, offset & ~1, tmp);
        return;
    }
    m5206_mbar_write(s, offset, value, 1);
}
