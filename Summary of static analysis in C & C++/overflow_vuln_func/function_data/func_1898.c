static struct pxa2xx_fir_s *pxa2xx_fir_init(target_phys_addr_t base,
                qemu_irq irq, struct pxa2xx_dma_state_s *dma,
                CharDriverState *chr)
{
    int iomemtype;
    struct pxa2xx_fir_s *s = (struct pxa2xx_fir_s *)
            qemu_mallocz(sizeof(struct pxa2xx_fir_s));

    s->base = base;
    s->irq = irq;
    s->dma = dma;
    s->chr = chr;

    pxa2xx_fir_reset(s);

    iomemtype = cpu_register_io_memory(0, pxa2xx_fir_readfn,
                    pxa2xx_fir_writefn, s);
    cpu_register_physical_memory(s->base, 0xfff, iomemtype);

    if (chr)
        qemu_chr_add_handlers(chr, pxa2xx_fir_is_empty,
                        pxa2xx_fir_rx, pxa2xx_fir_event, s);

    register_savevm("pxa2xx_fir", 0, 0, pxa2xx_fir_save, pxa2xx_fir_load, s);

    return s;
}
