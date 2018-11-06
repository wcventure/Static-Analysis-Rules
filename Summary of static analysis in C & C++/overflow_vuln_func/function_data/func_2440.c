static uint32_t m5206_mbar_readl(void *opaque, target_phys_addr_t offset)
{
    m5206_mbar_state *s = (m5206_mbar_state *)opaque;
    int width;
    offset &= 0x3ff;
    if (offset > 0x200) {
        hw_error("Bad MBAR read offset 0x%x", (int)offset);
    }
    width = m5206_mbar_width[offset >> 2];
    if (width < 4) {
        uint32_t val;
        val = m5206_mbar_readw(opaque, offset) << 16;
        val |= m5206_mbar_readw(opaque, offset + 2);
        return val;
    }
    return m5206_mbar_read(s, offset, 4);
}
