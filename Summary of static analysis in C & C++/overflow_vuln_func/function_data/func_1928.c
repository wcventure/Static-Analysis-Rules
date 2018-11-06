static void vfio_vga_probe_ati_3c3_quirk(VFIOPCIDevice *vdev)
{
    VFIOQuirk *quirk;

    /
    if (!vfio_pci_is(vdev, PCI_VENDOR_ID_ATI, PCI_ANY_ID) ||
        !vdev->bars[4].ioport || vdev->bars[4].region.size < 256) {
        return;
    }

    quirk = g_malloc0(sizeof(*quirk));
    quirk->mem = g_malloc0(sizeof(MemoryRegion));
    quirk->nr_mem = 1;

    memory_region_init_io(quirk->mem, OBJECT(vdev), &vfio_ati_3c3_quirk, vdev,
                          "vfio-ati-3c3-quirk", 1);
    memory_region_add_subregion(&vdev->vga.region[QEMU_PCI_VGA_IO_HI].mem,
                                3 /, quirk->mem);

    QLIST_INSERT_HEAD(&vdev->vga.region[QEMU_PCI_VGA_IO_HI].quirks,
                      quirk, next);

    trace_vfio_quirk_ati_3c3_probe(vdev->vbasedev.name);
}
