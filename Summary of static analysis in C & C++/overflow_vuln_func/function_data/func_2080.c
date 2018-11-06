void pci_data_write(PCIBus *s, uint32_t addr, uint32_t val, int len)
{
    PCIDevice *pci_dev = pci_dev_find_by_addr(s, addr);
    uint32_t config_addr = addr & (PCI_CONFIG_SPACE_SIZE - 1);

    if (!pci_dev)
        return;

    PCI_DPRINTF("%s: %s: addr=%02" PRIx32 " val=%08" PRIx32 " len=%d\n",
                __func__, pci_dev->name, config_addr, val, len);
    pci_dev->config_write(pci_dev, config_addr, val, len);
}
