static uint32_t pcie_mmcfg_data_read(PCIBus *s, uint32_t addr, int len)
{
    PCIDevice *pci_dev = pcie_dev_find_by_mmcfg_addr(s, addr);

    assert(len == 1 || len == 2 || len == 4);
    if (!pci_dev) {
        return ~0x0;
    }
    return pci_dev->config_read(pci_dev, PCIE_MMCFG_CONFOFFSET(addr), len);
}
