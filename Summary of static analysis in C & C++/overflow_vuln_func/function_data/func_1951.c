int pci_piix3_xen_ide_unplug(DeviceState *dev)
{
    PCIIDEState *pci_ide;
    DriveInfo *di;
    int i = 0;

    pci_ide = PCI_IDE(dev);

    for (; i < 3; i++) {
        di = drive_get_by_index(IF_IDE, i);
        if (di != NULL && !di->media_cd) {
            BlockBackend *blk = blk_by_legacy_dinfo(di);
            DeviceState *ds = blk_get_attached_dev(blk);
            if (ds) {
                blk_detach_dev(blk, ds);
            }
            pci_ide->bus[di->bus].ifs[di->unit].blk = NULL;
            blk_unref(blk);
        }
    }
    qdev_reset_all(DEVICE(dev));
    return 0;
}
