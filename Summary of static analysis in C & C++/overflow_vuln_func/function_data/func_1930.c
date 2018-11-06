static void vfio_probe_ati_bar2_quirk(VFIOPCIDevice *vdev, int nr)
{
    VFIOQuirk *quirk;
    VFIOConfigMirrorQuirk *mirror;

    /
    if (!vfio_pci_is(vdev, PCI_VENDOR_ID_ATI, PCI_ANY_ID) ||
        !vdev->has_vga || nr != 2 || !vdev->bars[2].mem64) {
        return;
    }

    quirk = g_malloc0(sizeof(*quirk));
    mirror = quirk->data = g_malloc0(sizeof(*mirror));
    mirror->mem = quirk->mem = g_malloc0(sizeof(MemoryRegion));
    quirk->nr_mem = 1;
    mirror->vdev = vdev;
    mirror->offset = 0x4000;
    mirror->bar = nr;

    memory_region_init_io(mirror->mem, OBJECT(vdev),
                          &vfio_generic_mirror_quirk, mirror,
                          "vfio-ati-bar2-4000-quirk", PCI_CONFIG_SPACE_SIZE);
    memory_region_add_subregion_overlap(&vdev->bars[nr].region.mem,
                                        mirror->offset, mirror->mem, 1);

    QLIST_INSERT_HEAD(&vdev->bars[nr].quirks, quirk, next);

    trace_vfio_quirk_ati_bar2_probe(vdev->vbasedev.name);
}
