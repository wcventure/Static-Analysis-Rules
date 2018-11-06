static void register_subpage(MemoryRegionSection *section)
{
    subpage_t *subpage;
    target_phys_addr_t base = section->offset_within_address_space
        & TARGET_PAGE_MASK;
    MemoryRegionSection *existing = phys_page_find(base >> TARGET_PAGE_BITS);
    MemoryRegionSection subsection = {
        .offset_within_address_space = base,
        .size = TARGET_PAGE_SIZE,
    };
    target_phys_addr_t start, end;

    assert(existing->mr->subpage || existing->mr == &io_mem_unassigned);

    if (!(existing->mr->subpage)) {
        subpage = subpage_init(base);
        subsection.mr = &subpage->iomem;
        phys_page_set(base >> TARGET_PAGE_BITS, 1,
                      phys_section_add(&subsection));
    } else {
        subpage = container_of(existing->mr, subpage_t, iomem);
    }
    start = section->offset_within_address_space & ~TARGET_PAGE_MASK;
    end = start + section->size;
    subpage_register(subpage, start, end, phys_section_add(section));
}
