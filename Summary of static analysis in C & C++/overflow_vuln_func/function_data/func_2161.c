static uint32_t pci_reg_read4(void *opaque, target_phys_addr_t addr)
{
    PPCE500PCIState *pci = opaque;
    unsigned long win;
    uint32_t value = 0;

    win = addr & 0xfe0;

    switch (win) {
    case PPCE500_PCI_OW1:
    case PPCE500_PCI_OW2:
    case PPCE500_PCI_OW3:
    case PPCE500_PCI_OW4:
        switch (addr & 0xC) {
        case PCI_POTAR:
            value = pci->pob[(addr >> 5) & 0x7].potar;
            break;
        case PCI_POTEAR:
            value = pci->pob[(addr >> 5) & 0x7].potear;
            break;
        case PCI_POWBAR:
            value = pci->pob[(addr >> 5) & 0x7].powbar;
            break;
        case PCI_POWAR:
            value = pci->pob[(addr >> 5) & 0x7].powar;
            break;
        default:
            break;
        }
        break;

    case PPCE500_PCI_IW3:
    case PPCE500_PCI_IW2:
    case PPCE500_PCI_IW1:
        switch (addr & 0xC) {
        case PCI_PITAR:
            value = pci->pib[(addr >> 5) & 0x3].pitar;
            break;
        case PCI_PIWBAR:
            value = pci->pib[(addr >> 5) & 0x3].piwbar;
            break;
        case PCI_PIWBEAR:
            value = pci->pib[(addr >> 5) & 0x3].piwbear;
            break;
        case PCI_PIWAR:
            value = pci->pib[(addr >> 5) & 0x3].piwar;
            break;
        default:
            break;
        };
        break;

    case PPCE500_PCI_GASKET_TIMR:
        value = pci->gasket_time;
        break;

    default:
        break;
    }

    pci_debug("%s: win:%lx(addr:" TARGET_FMT_plx ") -> value:%x\n", __func__,
              win, addr, value);
    return value;
}
