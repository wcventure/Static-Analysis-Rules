static int kvm_get_dirty_pages_log_range(MemoryRegionSection *section,
                                         unsigned long *bitmap)
{
    unsigned int i, j;
    unsigned long page_number, addr, addr1, c;
    unsigned int len = ((section->size / TARGET_PAGE_SIZE) + HOST_LONG_BITS - 1) / HOST_LONG_BITS;

    /
    for (i = 0; i < len; i++) {
        if (bitmap[i] != 0) {
            c = leul_to_cpu(bitmap[i]);
            do {
                j = ffsl(c) - 1;
                c &= ~(1ul << j);
                page_number = i * HOST_LONG_BITS + j;
                addr1 = page_number * TARGET_PAGE_SIZE;
                addr = section->offset_within_region + addr1;
                memory_region_set_dirty(section->mr, addr);
            } while (c != 0);
        }
    }
    return 0;
}
