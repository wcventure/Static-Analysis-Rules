static void vfio_probe_nvidia_bar5_quirk(VFIOPCIDevice *vdev, int nr)
{
    VFIOQuirk *quirk;
    VFIONvidiaBAR5Quirk *bar5;
    VFIOConfigWindowQuirk *window;

    if (!vfio_pci_is(vdev, PCI_VENDOR_ID_NVIDIA, PCI_ANY_ID) ||
        !vdev->has_vga || nr != 5) {
        return;
    }

    quirk = g_malloc0(sizeof(*quirk));
    quirk->mem = g_malloc0(sizeof(MemoryRegion) * 4);
    quirk->nr_mem = 4;
    bar5 = quirk->data = g_malloc0(sizeof(*bar5) +
                                   (sizeof(VFIOConfigWindowMatch) * 2));
    window = &bar5->window;

    window->vdev = vdev;
    window->address_offset = 0x8;
    window->data_offset = 0xc;
    window->nr_matches = 2;
    window->matches[0].match = 0x1800;
    window->matches[0].mask = PCI_CONFIG_SPACE_SIZE - 1;
    window->matches[1].match = 0x88000;
    window->matches[1].mask = PCIE_CONFIG_SPACE_SIZE - 1;
    window->bar = nr;
    window->addr_mem = bar5->addr_mem = &quirk->mem[0];
    window->data_mem = bar5->data_mem = &quirk->mem[1];

    memory_region_init_io(window->addr_mem, OBJECT(vdev),
                          &vfio_generic_window_address_quirk, window,
                          "vfio-nvidia-bar5-window-address-quirk", 4);
    memory_region_add_subregion_overlap(&vdev->bars[nr].region.mem,
                                        window->address_offset,
                                        window->addr_mem, 1);
    memory_region_set_enabled(window->addr_mem, false);

    memory_region_init_io(window->data_mem, OBJECT(vdev),
                          &vfio_generic_window_data_quirk, window,
                          "vfio-nvidia-bar5-window-data-quirk", 4);
    memory_region_add_subregion_overlap(&vdev->bars[nr].region.mem,
                                        window->data_offset,
                                        window->data_mem, 1);
    memory_region_set_enabled(window->data_mem, false);

    memory_region_init_io(&quirk->mem[2], OBJECT(vdev),
                          &vfio_nvidia_bar5_quirk_master, bar5,
                          "vfio-nvidia-bar5-master-quirk", 4);
    memory_region_add_subregion_overlap(&vdev->bars[nr].region.mem,
                                        0, &quirk->mem[2], 1);

    memory_region_init_io(&quirk->mem[3], OBJECT(vdev),
                          &vfio_nvidia_bar5_quirk_enable, bar5,
                          "vfio-nvidia-bar5-enable-quirk", 4);
    memory_region_add_subregion_overlap(&vdev->bars[nr].region.mem,
                                        4, &quirk->mem[3], 1);

    QLIST_INSERT_HEAD(&vdev->bars[nr].quirks, quirk, next);

    trace_vfio_quirk_nvidia_bar5_probe(vdev->vbasedev.name);
}
