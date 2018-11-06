struct pxa2xx_mmci_s *pxa2xx_mmci_init(target_phys_addr_t base,
                qemu_irq irq, void *dma)
{
    int iomemtype;
    struct pxa2xx_mmci_s *s;

    s = (struct pxa2xx_mmci_s *) qemu_mallocz(sizeof(struct pxa2xx_mmci_s));
    s->base = base;
    s->irq = irq;
    s->dma = dma;

    iomemtype = cpu_register_io_memory(0, pxa2xx_mmci_readfn,
                    pxa2xx_mmci_writefn, s);
    cpu_register_physical_memory(base, 0x000fffff, iomemtype);

    /
    s->card = sd_init(sd_bdrv);

    register_savevm("pxa2xx_mmci", 0, 0,
                    pxa2xx_mmci_save, pxa2xx_mmci_load, s);

    return s;
}
