static uint32_t pcie_mmcfg_data_read(PCIBus *s, uint32_t addr, int len)
{
    PCIDevice *pci_dev = pcie_dev_find_by_mmcfg_addr(s, addr);

    if (!pci_dev) {
        return ~0x0;
    }
    return pci_host_config_read_common(pci_dev, PCIE_MMCFG_CONFOFFSET(addr),
                                       pci_config_size(pci_dev), len);
}
