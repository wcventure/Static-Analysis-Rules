static void m5206_mbar_writel(void *opaque, target_phys_addr_t offset,
                              uint32_t value)
{
    m5206_mbar_state *s = (m5206_mbar_state *)opaque;
    int width;
    offset &= 0x3ff;
    if (offset > 0x200) {
        hw_error("Bad MBAR write offset 0x%x", (int)offset);
    }
    width = m5206_mbar_width[offset >> 2];
    if (width < 4) {
        m5206_mbar_writew(opaque, offset, value >> 16);
        m5206_mbar_writew(opaque, offset + 2, value & 0xffff);
        return;
    }
    m5206_mbar_write(s, offset, value, 4);
}
