static void vfio_probe_rtl8168_bar2_quirk(VFIOPCIDevice *vdev, int nr)
{
    VFIOQuirk *quirk;
    VFIOrtl8168Quirk *rtl;

    if (!vfio_pci_is(vdev, PCI_VENDOR_ID_REALTEK, 0x8168) || nr != 2) {
        return;
    }

    quirk = g_malloc0(sizeof(*quirk));
    quirk->mem = g_malloc0(sizeof(MemoryRegion) * 2);
    quirk->nr_mem = 2;
    quirk->data = rtl = g_malloc0(sizeof(*rtl));
    rtl->vdev = vdev;

    memory_region_init_io(&quirk->mem[0], OBJECT(vdev),
                          &vfio_rtl_address_quirk, rtl,
                          "vfio-rtl8168-window-address-quirk", 4);
    memory_region_add_subregion_overlap(&vdev->bars[nr].region.mem,
                                        0x74, &quirk->mem[0], 1);

    memory_region_init_io(&quirk->mem[1], OBJECT(vdev),
                          &vfio_rtl_data_quirk, rtl,
                          "vfio-rtl8168-window-data-quirk", 4);
    memory_region_add_subregion_overlap(&vdev->bars[nr].region.mem,
                                        0x70, &quirk->mem[1], 1);

    QLIST_INSERT_HEAD(&vdev->bars[nr].quirks, quirk, next);

    trace_vfio_quirk_rtl8168_probe(vdev->vbasedev.name);
}
