static struct pxa2xx_dma_state_s *pxa2xx_dma_init(target_phys_addr_t base,
                qemu_irq irq, int channels)
{
    int i, iomemtype;
    struct pxa2xx_dma_state_s *s;
    s = (struct pxa2xx_dma_state_s *)
            qemu_mallocz(sizeof(struct pxa2xx_dma_state_s));

    s->channels = channels;
    s->chan = qemu_mallocz(sizeof(struct pxa2xx_dma_channel_s) * s->channels);
    s->base = base;
    s->irq = irq;
    s->handler = (pxa2xx_dma_handler_t) pxa2xx_dma_request;
    s->req = qemu_mallocz(sizeof(uint8_t) * PXA2XX_DMA_NUM_REQUESTS);

    memset(s->chan, 0, sizeof(struct pxa2xx_dma_channel_s) * s->channels);
    for (i = 0; i < s->channels; i ++)
        s->chan[i].state = DCSR_STOPINTR;

    memset(s->req, 0, sizeof(uint8_t) * PXA2XX_DMA_NUM_REQUESTS);

    iomemtype = cpu_register_io_memory(0, pxa2xx_dma_readfn,
                    pxa2xx_dma_writefn, s);
    cpu_register_physical_memory(base, 0x0000ffff, iomemtype);

    register_savevm("pxa2xx_dma", 0, 0, pxa2xx_dma_save, pxa2xx_dma_load, s);

    return s;
}
