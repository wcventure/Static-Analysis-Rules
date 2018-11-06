qemu_irq *pl190_init(uint32_t base, qemu_irq irq, qemu_irq fiq)
{
    pl190_state *s;
    qemu_irq *qi;
    int iomemtype;

    s = (pl190_state *)qemu_mallocz(sizeof(pl190_state));
    iomemtype = cpu_register_io_memory(0, pl190_readfn,
                                       pl190_writefn, s);
    cpu_register_physical_memory(base, 0x00000fff, iomemtype);
    qi = qemu_allocate_irqs(pl190_set_irq, s, 32);
    s->base = base;
    s->irq = irq;
    s->fiq = fiq;
    pl190_reset(s);
    /
    return qi;
}
