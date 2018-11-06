static void pci_reg_write4(void *opaque, target_phys_addr_t addr,
                               uint32_t value)
{
    PPCE500PCIState *pci = opaque;
    unsigned long win;

    win = addr & 0xfe0;

    pci_debug("%s: value:%x -> win:%lx(addr:" TARGET_FMT_plx ")\n",
              __func__, value, win, addr);

    switch (win) {
    case PPCE500_PCI_OW1:
    case PPCE500_PCI_OW2:
    case PPCE500_PCI_OW3:
    case PPCE500_PCI_OW4:
        switch (addr & 0xC) {
        case PCI_POTAR:
            pci->pob[(addr >> 5) & 0x7].potar = value;
            break;
        case PCI_POTEAR:
            pci->pob[(addr >> 5) & 0x7].potear = value;
            break;
        case PCI_POWBAR:
            pci->pob[(addr >> 5) & 0x7].powbar = value;
            break;
        case PCI_POWAR:
            pci->pob[(addr >> 5) & 0x7].powar = value;
            break;
        default:
            break;
        };
        break;

    case PPCE500_PCI_IW3:
    case PPCE500_PCI_IW2:
    case PPCE500_PCI_IW1:
        switch (addr & 0xC) {
        case PCI_PITAR:
            pci->pib[(addr >> 5) & 0x3].pitar = value;
            break;
        case PCI_PIWBAR:
            pci->pib[(addr >> 5) & 0x3].piwbar = value;
            break;
        case PCI_PIWBEAR:
            pci->pib[(addr >> 5) & 0x3].piwbear = value;
            break;
        case PCI_PIWAR:
            pci->pib[(addr >> 5) & 0x3].piwar = value;
            break;
        default:
            break;
        };
        break;

    case PPCE500_PCI_GASKET_TIMR:
        pci->gasket_time = value;
        break;

    default:
        break;
    };
}
