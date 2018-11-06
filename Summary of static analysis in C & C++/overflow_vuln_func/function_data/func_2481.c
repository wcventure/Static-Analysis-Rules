static void cirrus_mem_writeb_mode4and5_8bpp(CirrusVGAState * s,
					     unsigned mode,
					     unsigned offset,
					     uint32_t mem_value)
{
    int x;
    unsigned val = mem_value;
    uint8_t *dst;

    dst = s->vga.vram_ptr + (offset &= s->cirrus_addr_mask);
    for (x = 0; x < 8; x++) {
	if (val & 0x80) {
	    *dst = s->cirrus_shadow_gr1;
	} else if (mode == 5) {
	    *dst = s->cirrus_shadow_gr0;
	}
	val <<= 1;
	dst++;
    }
    memory_region_set_dirty(&s->vga.vram, offset, 8);
}
