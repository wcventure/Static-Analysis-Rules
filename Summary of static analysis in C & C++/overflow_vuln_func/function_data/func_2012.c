QPCIBus *qpci_init_spapr(QGuestAllocator *alloc)
{
    QPCIBusSPAPR *ret;

    ret = g_malloc(sizeof(*ret));

    ret->alloc = alloc;

    ret->bus.io_readb = qpci_spapr_io_readb;
    ret->bus.io_readw = qpci_spapr_io_readw;
    ret->bus.io_readl = qpci_spapr_io_readl;

    ret->bus.io_writeb = qpci_spapr_io_writeb;
    ret->bus.io_writew = qpci_spapr_io_writew;
    ret->bus.io_writel = qpci_spapr_io_writel;

    ret->bus.config_readb = qpci_spapr_config_readb;
    ret->bus.config_readw = qpci_spapr_config_readw;
    ret->bus.config_readl = qpci_spapr_config_readl;

    ret->bus.config_writeb = qpci_spapr_config_writeb;
    ret->bus.config_writew = qpci_spapr_config_writew;
    ret->bus.config_writel = qpci_spapr_config_writel;

    ret->bus.iomap = qpci_spapr_iomap;
    ret->bus.iounmap = qpci_spapr_iounmap;

    /
    ret->buid = 0x800000020000000ULL;

    ret->pio_cpu_base = SPAPR_PCI_WINDOW_BASE + SPAPR_PCI_IO_WIN_OFF;
    ret->pio.pci_base = 0;
    ret->pio.size = SPAPR_PCI_IO_WIN_SIZE;

    /
    ret->mmio32_cpu_base = SPAPR_PCI_WINDOW_BASE + SPAPR_PCI_MMIO32_WIN_OFF;
    ret->mmio32.pci_base = 0x80000000; /
    ret->mmio32.size = SPAPR_PCI_MMIO32_WIN_SIZE;

    ret->pci_hole_start = 0xC0000000;
    ret->pci_hole_size =
        ret->mmio32.pci_base + ret->mmio32.size - ret->pci_hole_start;
    ret->pci_hole_alloc = 0;

    ret->pci_iohole_start = 0xc000;
    ret->pci_iohole_size =
        ret->pio.pci_base + ret->pio.size - ret->pci_iohole_start;
    ret->pci_iohole_alloc = 0;

    return &ret->bus;
}
