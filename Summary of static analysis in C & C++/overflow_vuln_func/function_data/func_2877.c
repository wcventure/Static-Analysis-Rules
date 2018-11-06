static void vfio_listener_region_add(MemoryListener *listener,
                                     MemoryRegionSection *section)
{
    VFIOContainer *container = container_of(listener, VFIOContainer,
                                            iommu_data.listener);
    hwaddr iova, end;
    void *vaddr;
    int ret;

    assert(!memory_region_is_iommu(section->mr));

    if (vfio_listener_skipped_section(section)) {
        DPRINTF("SKIPPING region_add %"HWADDR_PRIx" - %"PRIx64"\n",
                section->offset_within_address_space,
                section->offset_within_address_space + section->size - 1);
        return;
    }

    if (unlikely((section->offset_within_address_space & ~TARGET_PAGE_MASK) !=
                 (section->offset_within_region & ~TARGET_PAGE_MASK))) {
        error_report("%s received unaligned region", __func__);
        return;
    }

    iova = TARGET_PAGE_ALIGN(section->offset_within_address_space);
    end = (section->offset_within_address_space + int128_get64(section->size)) &
          TARGET_PAGE_MASK;

    if (iova >= end) {
        return;
    }

    vaddr = memory_region_get_ram_ptr(section->mr) +
            section->offset_within_region +
            (iova - section->offset_within_address_space);

    DPRINTF("region_add %"HWADDR_PRIx" - %"HWADDR_PRIx" [%p]\n",
            iova, end - 1, vaddr);

    memory_region_ref(section->mr);
    ret = vfio_dma_map(container, iova, end - iova, vaddr, section->readonly);
    if (ret) {
        error_report("vfio_dma_map(%p, 0x%"HWADDR_PRIx", "
                     "0x%"HWADDR_PRIx", %p) = %d (%m)",
                     container, iova, end - iova, vaddr, ret);
    }
}
