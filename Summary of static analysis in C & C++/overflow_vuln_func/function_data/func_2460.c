static void vfio_listener_region_add(MemoryListener *listener,
                                     MemoryRegionSection *section)
{
    VFIOContainer *container = container_of(listener, VFIOContainer, listener);
    hwaddr iova, end;
    Int128 llend;
    void *vaddr;
    int ret;

    if (vfio_listener_skipped_section(section)) {
        trace_vfio_listener_region_add_skip(
                section->offset_within_address_space,
                section->offset_within_address_space +
                int128_get64(int128_sub(section->size, int128_one())));
        return;
    }

    if (unlikely((section->offset_within_address_space & ~TARGET_PAGE_MASK) !=
                 (section->offset_within_region & ~TARGET_PAGE_MASK))) {
        error_report("%s received unaligned region", __func__);
        return;
    }

    iova = TARGET_PAGE_ALIGN(section->offset_within_address_space);
    llend = int128_make64(section->offset_within_address_space);
    llend = int128_add(llend, section->size);
    llend = int128_and(llend, int128_exts64(TARGET_PAGE_MASK));

    if (int128_ge(int128_make64(iova), llend)) {
        return;
    }
    end = int128_get64(llend);

    if ((iova < container->min_iova) || ((end - 1) > container->max_iova)) {
        error_report("vfio: IOMMU container %p can't map guest IOVA region"
                     " 0x%"HWADDR_PRIx"..0x%"HWADDR_PRIx,
                     container, iova, end - 1);
        ret = -EFAULT;
        goto fail;
    }

    memory_region_ref(section->mr);

    if (memory_region_is_iommu(section->mr)) {
        VFIOGuestIOMMU *giommu;

        trace_vfio_listener_region_add_iommu(iova, end - 1);
        /
        giommu = g_malloc0(sizeof(*giommu));
        giommu->iommu = section->mr;
        giommu->container = container;
        giommu->n.notify = vfio_iommu_map_notify;
        QLIST_INSERT_HEAD(&container->giommu_list, giommu, giommu_next);

        memory_region_register_iommu_notifier(giommu->iommu, &giommu->n);
        memory_region_iommu_replay(giommu->iommu, &giommu->n,
                                   vfio_container_granularity(container),
                                   false);

        return;
    }

    /

    vaddr = memory_region_get_ram_ptr(section->mr) +
            section->offset_within_region +
            (iova - section->offset_within_address_space);

    trace_vfio_listener_region_add_ram(iova, end - 1, vaddr);

    ret = vfio_dma_map(container, iova, end - iova, vaddr, section->readonly);
    if (ret) {
        error_report("vfio_dma_map(%p, 0x%"HWADDR_PRIx", "
                     "0x%"HWADDR_PRIx", %p) = %d (%m)",
                     container, iova, end - iova, vaddr, ret);
        goto fail;
    }

    return;

fail:
    /
    if (!container->initialized) {
        if (!container->error) {
            container->error = ret;
        }
    } else {
        hw_error("vfio: DMA mapping failed, unable to continue");
    }
}
