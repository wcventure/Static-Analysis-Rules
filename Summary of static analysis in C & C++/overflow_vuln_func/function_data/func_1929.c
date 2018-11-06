static void vfio_probe_ati_bar4_quirk(VFIOPCIDevice *vdev, int nr)
{
    VFIOQuirk *quirk;
    VFIOConfigWindowQuirk *window;

    /
    if (!vfio_pci_is(vdev, PCI_VENDOR_ID_ATI, PCI_ANY_ID) ||
        !vdev->has_vga || nr != 4) {
        return;
    }

    quirk = g_malloc0(sizeof(*quirk));
    quirk->mem = g_malloc0(sizeof(MemoryRegion) * 2);
    quirk->nr_mem = 2;
    window = quirk->data = g_malloc0(sizeof(*window) +
                                     sizeof(VFIOConfigWindowMatch));
    window->vdev = vdev;
    window->address_offset = 0;
    window->data_offset = 4;
    window->nr_matches = 1;
    window->matches[0].match = 0x4000;
    window->matches[0].mask = PCIE_CONFIG_SPACE_SIZE - 1;
    window->bar = nr;
    window->addr_mem = &quirk->mem[0];
    window->data_mem = &quirk->mem[1];

    memory_region_init_io(window->addr_mem, OBJECT(vdev),
                          &vfio_generic_window_address_quirk, window,
                          "vfio-ati-bar4-window-address-quirk", 4);
    memory_region_add_subregion_overlap(&vdev->bars[nr].region.mem,
                                        window->address_offset,
                                        window->addr_mem, 1);

    memory_region_init_io(window->data_mem, OBJECT(vdev),
                          &vfio_generic_window_data_quirk, window,
                          "vfio-ati-bar4-window-data-quirk", 4);
    memory_region_add_subregion_overlap(&vdev->bars[nr].region.mem,
                                        window->data_offset,
                                        window->data_mem, 1);

    QLIST_INSERT_HEAD(&vdev->bars[nr].quirks, quirk, next);

    trace_vfio_quirk_ati_bar4_probe(vdev->vbasedev.name);
}
