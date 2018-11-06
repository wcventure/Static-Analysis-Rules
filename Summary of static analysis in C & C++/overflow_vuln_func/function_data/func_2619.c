static int vfio_initfn(PCIDevice *pdev)
{
    VFIODevice *pvdev, *vdev = DO_UPCAST(VFIODevice, pdev, pdev);
    VFIOGroup *group;
    char path[PATH_MAX], iommu_group_path[PATH_MAX], *group_name;
    ssize_t len;
    struct stat st;
    int groupid;
    int ret;

    /
    snprintf(path, sizeof(path),
             "/sys/bus/pci/devices/%04x:%02x:%02x.%01x/",
             vdev->host.domain, vdev->host.bus, vdev->host.slot,
             vdev->host.function);
    if (stat(path, &st) < 0) {
        error_report("vfio: error: no such host device: %s", path);
        return -errno;
    }

    strncat(path, "iommu_group", sizeof(path) - strlen(path) - 1);

    len = readlink(path, iommu_group_path, PATH_MAX);
    if (len <= 0) {
        error_report("vfio: error no iommu_group for device");
        return -errno;
    }

    iommu_group_path[len] = 0;
    group_name = basename(iommu_group_path);

    if (sscanf(group_name, "%d", &groupid) != 1) {
        error_report("vfio: error reading %s: %m", path);
        return -errno;
    }

    DPRINTF("%s(%04x:%02x:%02x.%x) group %d\n", __func__, vdev->host.domain,
            vdev->host.bus, vdev->host.slot, vdev->host.function, groupid);

    group = vfio_get_group(groupid);
    if (!group) {
        error_report("vfio: failed to get group %d", groupid);
        return -ENOENT;
    }

    snprintf(path, sizeof(path), "%04x:%02x:%02x.%01x",
            vdev->host.domain, vdev->host.bus, vdev->host.slot,
            vdev->host.function);

    QLIST_FOREACH(pvdev, &group->device_list, next) {
        if (pvdev->host.domain == vdev->host.domain &&
            pvdev->host.bus == vdev->host.bus &&
            pvdev->host.slot == vdev->host.slot &&
            pvdev->host.function == vdev->host.function) {

            error_report("vfio: error: device %s is already attached", path);
            vfio_put_group(group);
            return -EBUSY;
        }
    }

    ret = vfio_get_device(group, path, vdev);
    if (ret) {
        error_report("vfio: failed to get device %s", path);
        vfio_put_group(group);
        return ret;
    }

    /
    ret = pread(vdev->fd, vdev->pdev.config,
                MIN(pci_config_size(&vdev->pdev), vdev->config_size),
                vdev->config_offset);
    if (ret < (int)MIN(pci_config_size(&vdev->pdev), vdev->config_size)) {
        ret = ret < 0 ? -errno : -EFAULT;
        error_report("vfio: Failed to read device config space");
        goto out_put;
    }

    /
    vdev->emulated_config_bits = g_malloc0(vdev->config_size);

    /
    memset(vdev->emulated_config_bits + PCI_ROM_ADDRESS, 0xff, 4);

    /
    vdev->emulated_config_bits[PCI_HEADER_TYPE] =
                                              PCI_HEADER_TYPE_MULTI_FUNCTION;

    /
    if (vdev->pdev.cap_present & QEMU_PCI_CAP_MULTIFUNCTION) {
        vdev->pdev.config[PCI_HEADER_TYPE] |= PCI_HEADER_TYPE_MULTI_FUNCTION;
    } else {
        vdev->pdev.config[PCI_HEADER_TYPE] &= ~PCI_HEADER_TYPE_MULTI_FUNCTION;
    }

    /
    memset(&vdev->pdev.config[PCI_BASE_ADDRESS_0], 0, 24);
    memset(&vdev->pdev.config[PCI_ROM_ADDRESS], 0, 4);

    vfio_pci_size_rom(vdev);

    ret = vfio_early_setup_msix(vdev);
    if (ret) {
        goto out_put;
    }

    vfio_map_bars(vdev);

    ret = vfio_add_capabilities(vdev);
    if (ret) {
        goto out_teardown;
    }

    /
    if (pdev->cap_present & QEMU_PCI_CAP_MSIX) {
        memset(vdev->emulated_config_bits + pdev->msix_cap, 0xff,
               MSIX_CAP_LENGTH);
    }

    if (pdev->cap_present & QEMU_PCI_CAP_MSI) {
        memset(vdev->emulated_config_bits + pdev->msi_cap, 0xff,
               vdev->msi_cap_size);
    }

    if (vfio_pci_read_config(&vdev->pdev, PCI_INTERRUPT_PIN, 1)) {
        vdev->intx.mmap_timer = timer_new_ms(QEMU_CLOCK_VIRTUAL,
                                                  vfio_intx_mmap_enable, vdev);
        pci_device_set_intx_routing_notifier(&vdev->pdev, vfio_update_irq);
        ret = vfio_enable_intx(vdev);
        if (ret) {
            goto out_teardown;
        }
    }

    add_boot_device_path(vdev->bootindex, &pdev->qdev, NULL);
    vfio_register_err_notifier(vdev);

    return 0;

out_teardown:
    pci_device_set_intx_routing_notifier(&vdev->pdev, NULL);
    vfio_teardown_msi(vdev);
    vfio_unmap_bars(vdev);
out_put:
    g_free(vdev->emulated_config_bits);
    vfio_put_device(vdev);
    vfio_put_group(group);
    return ret;
}
