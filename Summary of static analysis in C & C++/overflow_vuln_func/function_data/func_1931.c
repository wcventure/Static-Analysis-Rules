static void vfio_vga_probe_nvidia_3d0_quirk(VFIOPCIDevice *vdev)
{
    VFIOQuirk *quirk;
    VFIONvidia3d0Quirk *data;

    if (!vfio_pci_is(vdev, PCI_VENDOR_ID_NVIDIA, PCI_ANY_ID) ||
        !vdev->bars[1].region.size) {
        return;
    }

    quirk = g_malloc0(sizeof(*quirk));
    quirk->data = data = g_malloc0(sizeof(*data));
    quirk->mem = g_malloc0(sizeof(MemoryRegion) * 2);
    quirk->nr_mem = 2;
    data->vdev = vdev;

    memory_region_init_io(&quirk->mem[0], OBJECT(vdev), &vfio_nvidia_3d4_quirk,
                          data, "vfio-nvidia-3d4-quirk", 2);
    memory_region_add_subregion(&vdev->vga.region[QEMU_PCI_VGA_IO_HI].mem,
                                0x14 /, &quirk->mem[0]);

    memory_region_init_io(&quirk->mem[1], OBJECT(vdev), &vfio_nvidia_3d0_quirk,
                          data, "vfio-nvidia-3d0-quirk", 2);
    memory_region_add_subregion(&vdev->vga.region[QEMU_PCI_VGA_IO_HI].mem,
                                0x10 /, &quirk->mem[1]);

    QLIST_INSERT_HEAD(&vdev->vga.region[QEMU_PCI_VGA_IO_HI].quirks,
                      quirk, next);

    trace_vfio_quirk_nvidia_3d0_probe(vdev->vbasedev.name);
}
