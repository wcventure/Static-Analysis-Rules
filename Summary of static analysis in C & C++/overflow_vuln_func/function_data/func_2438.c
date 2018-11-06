static uint32_t m5206_mbar_readb(void *opaque, target_phys_addr_t offset)
{
    m5206_mbar_state *s = (m5206_mbar_state *)opaque;
    offset &= 0x3ff;
    if (offset > 0x200) {
        hw_error("Bad MBAR read offset 0x%x", (int)offset);
    }
    if (m5206_mbar_width[offset >> 2] > 1) {
        uint16_t val;
        val = m5206_mbar_readw(opaque, offset & ~1);
        if ((offset & 1) == 0) {
            val >>= 8;
        }
        return val & 0xff;
    }
    return m5206_mbar_read(s, offset, 1);
}
