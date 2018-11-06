static struct pxa2xx_i2s_s *pxa2xx_i2s_init(target_phys_addr_t base,
                qemu_irq irq, struct pxa2xx_dma_state_s *dma)
{
    int iomemtype;
    struct pxa2xx_i2s_s *s = (struct pxa2xx_i2s_s *)
            qemu_mallocz(sizeof(struct pxa2xx_i2s_s));

    s->base = base;
    s->irq = irq;
    s->dma = dma;
    s->data_req = pxa2xx_i2s_data_req;

    pxa2xx_i2s_reset(s);

    iomemtype = cpu_register_io_memory(0, pxa2xx_i2s_readfn,
                    pxa2xx_i2s_writefn, s);
    cpu_register_physical_memory(s->base & 0xfff00000, 0xfffff, iomemtype);

    register_savevm("pxa2xx_i2s", base, 0,
                    pxa2xx_i2s_save, pxa2xx_i2s_load, s);

    return s;
}
