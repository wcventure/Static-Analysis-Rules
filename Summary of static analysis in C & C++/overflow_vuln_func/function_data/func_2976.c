uint32_t pci_default_read_config(PCIDevice *d, 
                                 uint32_t address, int len)
{
    uint32_t val;
    switch(len) {
    case 1:
        val = d->config[address];
        break;
    case 2:
        val = le16_to_cpu(*(uint16_t *)(d->config + address));
        break;
    default:
    case 4:
        val = le32_to_cpu(*(uint32_t *)(d->config + address));
        break;
    }
    return val;
}
