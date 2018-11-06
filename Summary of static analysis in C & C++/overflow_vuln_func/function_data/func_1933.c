static void vfio_probe_nvidia_bar0_quirk(VFIOPCIDevice *vdev, int nr)
{
    VFIOQuirk *quirk;
    VFIOConfigMirrorQuirk *mirror;

    if (!vfio_pci_is(vdev, PCI_VENDOR_ID_NVIDIA, PCI_ANY_ID) ||
        !vfio_is_vga(vdev) || nr != 0) {
        return;
    }

    quirk = g_malloc0(sizeof(*quirk));
    mirror = quirk->data = g_malloc0(sizeof(*mirror));
    mirror->mem = quirk->mem = g_malloc0(sizeof(MemoryRegion));
    quirk->nr_mem = 1;
    mirror->vdev = vdev;
    mirror->offset = 0x88000;
    mirror->bar = nr;

    memory_region_init_io(mirror->mem, OBJECT(vdev),
                          &vfio_nvidia_mirror_quirk, mirror,
                          "vfio-nvidia-bar0-88000-mirror-quirk",
                          PCIE_CONFIG_SPACE_SIZE);
    memory_region_add_subregion_overlap(&vdev->bars[nr].region.mem,
                                        mirror->offset, mirror->mem, 1);

    QLIST_INSERT_HEAD(&vdev->bars[nr].quirks, quirk, next);

    /
    if (vdev->has_vga) {
        quirk = g_malloc0(sizeof(*quirk));
        mirror = quirk->data = g_malloc0(sizeof(*mirror));
        mirror->mem = quirk->mem = g_malloc0(sizeof(MemoryRegion));
        quirk->nr_mem = 1;
        mirror->vdev = vdev;
        mirror->offset = 0x1800;
        mirror->bar = nr;

        memory_region_init_io(mirror->mem, OBJECT(vdev),
                              &vfio_nvidia_mirror_quirk, mirror,
                              "vfio-nvidia-bar0-1800-mirror-quirk",
                              PCI_CONFIG_SPACE_SIZE);
        memory_region_add_subregion_overlap(&vdev->bars[nr].region.mem,
                                            mirror->offset, mirror->mem, 1);

        QLIST_INSERT_HEAD(&vdev->bars[nr].quirks, quirk, next);
    }

    trace_vfio_quirk_nvidia_bar0_probe(vdev->vbasedev.name);
}
