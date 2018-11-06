void *pl080_init(uint32_t base, qemu_irq irq, int nchannels)
{
    int iomemtype;
    pl080_state *s;

    s = (pl080_state *)qemu_mallocz(sizeof(pl080_state));
    iomemtype = cpu_register_io_memory(0, pl080_readfn,
                                       pl080_writefn, s);
    cpu_register_physical_memory(base, 0x00000fff, iomemtype);
    s->base = base;
    s->irq = irq;
    s->nchannels = nchannels;
    /
    return s;
}
