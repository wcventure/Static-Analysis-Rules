static void pcie_mmcfg_data_write(PCIBus *s,
                                  uint32_t mmcfg_addr, uint32_t val, int len)
{
    PCIDevice *pci_dev = pcie_dev_find_by_mmcfg_addr(s, mmcfg_addr);

    if (!pci_dev)
        return;

    pci_dev->config_write(pci_dev,
                          PCIE_MMCFG_CONFOFFSET(mmcfg_addr), val, len);
}
