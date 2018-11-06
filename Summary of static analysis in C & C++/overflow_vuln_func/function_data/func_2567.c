static void vfio_map_bar(VFIOPCIDevice *vdev, int nr)
{
    VFIOBAR *bar = &vdev->bars[nr];
    uint64_t size = bar->region.size;
    char name[64];
    uint32_t pci_bar;
    uint8_t type;
    int ret;

    /
    if (!size) {
        return;
    }

    snprintf(name, sizeof(name), "VFIO %04x:%02x:%02x.%x BAR %d",
             vdev->host.domain, vdev->host.bus, vdev->host.slot,
             vdev->host.function, nr);

    /
    ret = pread(vdev->vbasedev.fd, &pci_bar, sizeof(pci_bar),
                vdev->config_offset + PCI_BASE_ADDRESS_0 + (4 * nr));
    if (ret != sizeof(pci_bar)) {
        error_report("vfio: Failed to read BAR %d (%m)", nr);
        return;
    }

    pci_bar = le32_to_cpu(pci_bar);
    bar->ioport = (pci_bar & PCI_BASE_ADDRESS_SPACE_IO);
    bar->mem64 = bar->ioport ? 0 : (pci_bar & PCI_BASE_ADDRESS_MEM_TYPE_64);
    type = pci_bar & (bar->ioport ? ~PCI_BASE_ADDRESS_IO_MASK :
                                    ~PCI_BASE_ADDRESS_MEM_MASK);

    /
    memory_region_init_io(&bar->region.mem, OBJECT(vdev), &vfio_region_ops,
                          bar, name, size);
    pci_register_bar(&vdev->pdev, nr, type, &bar->region.mem);

    /
    if (vdev->msix && vdev->msix->table_bar == nr) {
        size = vdev->msix->table_offset & qemu_host_page_mask;
    }

    strncat(name, " mmap", sizeof(name) - strlen(name) - 1);
    if (vfio_mmap_region(OBJECT(vdev), &bar->region, &bar->region.mem,
                      &bar->region.mmap_mem, &bar->region.mmap,
                      size, 0, name)) {
        error_report("%s unsupported. Performance may be slow", name);
    }

    if (vdev->msix && vdev->msix->table_bar == nr) {
        uint64_t start;

        start = HOST_PAGE_ALIGN(vdev->msix->table_offset +
                                (vdev->msix->entries * PCI_MSIX_ENTRY_SIZE));

        size = start < bar->region.size ? bar->region.size - start : 0;
        strncat(name, " msix-hi", sizeof(name) - strlen(name) - 1);
        /
        if (vfio_mmap_region(OBJECT(vdev), &bar->region, &bar->region.mem,
                          &vdev->msix->mmap_mem,
                          &vdev->msix->mmap, size, start, name)) {
            error_report("%s unsupported. Performance may be slow", name);
        }
    }

    vfio_bar_quirk_setup(vdev, nr);
}
