{
    hwaddr src_len;
    uint8_t *dest;
    uint8_t *src;
    int first, last = 0;
    int dirty;
    int i;
    ram_addr_t addr;
    MemoryRegion *mem;

    i = *first_row;
    *first_row = -1;
    src_len = src_width * rows;

    mem = mem_section->mr;
    if (!mem) {
        return;
    }
    memory_region_sync_dirty_bitmap(mem);

    addr = mem_section->offset_within_region;
    src = memory_region_get_ram_ptr(mem) + addr;

    dest = surface_data(ds);
    if (dest_col_pitch < 0) {
        dest -= dest_col_pitch * (cols - 1);
    }
    if (dest_row_pitch < 0) {
        dest -= dest_row_pitch * (rows - 1);
    }
    first = -1;

    addr += i * src_width;
    src += i * src_width;
    dest += i * dest_row_pitch;

    for (; i < rows; i++) {
        dirty = memory_region_get_dirty(mem, addr, src_width,
                                             DIRTY_MEMORY_VGA);
        if (dirty || invalidate) {
            fn(opaque, dest, src, cols, dest_col_pitch);
            if (first == -1)
                first = i;
            last = i;
        }
        addr += src_width;
        src += src_width;
        dest += dest_row_pitch;
    }
    if (first < 0) {
        return;
    }
    memory_region_reset_dirty(mem, mem_section->offset_within_region, src_len,
                              DIRTY_MEMORY_VGA);
    *first_row = first;
    *last_row = last;
}
