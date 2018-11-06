void pci_default_write_config(PCIDevice *d, 
                              uint32_t address, uint32_t val, int len)
{
    int can_write, i;
    uint32_t end, addr;

    if (len == 4 && ((address >= 0x10 && address < 0x10 + 4 * 6) || 
                     (address >= 0x30 && address < 0x34))) {
        PCIIORegion *r;
        int reg;

        if ( address >= 0x30 ) {
            reg = PCI_ROM_SLOT;
        }else{
            reg = (address - 0x10) >> 2;
        }
        r = &d->io_regions[reg];
        if (r->size == 0)
            goto default_config;
        /
        if (reg == PCI_ROM_SLOT) {
            /
            val &= (~(r->size - 1)) | 1;
        } else {
            val &= ~(r->size - 1);
            val |= r->type;
        }
        *(uint32_t *)(d->config + address) = cpu_to_le32(val);
        pci_update_mappings(d);
        return;
    }
 default_config:
    /
    addr = address;
    for(i = 0; i < len; i++) {
        /
        switch(d->config[0x0e]) {
        case 0x00:
        case 0x80:
            switch(addr) {
            case 0x00:
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x08:
            case 0x09:
            case 0x0a:
            case 0x0b:
            case 0x0e:
            case 0x10 ... 0x27: /
            case 0x30 ... 0x33: /
            case 0x3d:
                can_write = 0;
                break;
            default:
                can_write = 1;
                break;
            }
            break;
        default:
        case 0x01:
            switch(addr) {
            case 0x00:
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x08:
            case 0x09:
            case 0x0a:
            case 0x0b:
            case 0x0e:
            case 0x38 ... 0x3b: /
            case 0x3d:
                can_write = 0;
                break;
            default:
                can_write = 1;
                break;
            }
            break;
        }
        if (can_write) {
            d->config[addr] = val;
        }
        addr++;
        val >>= 8;
    }

    end = address + len;
    if (end > PCI_COMMAND && address < (PCI_COMMAND + 2)) {
        /
        pci_update_mappings(d);
    }
}
