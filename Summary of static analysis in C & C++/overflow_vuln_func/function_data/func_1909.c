static qemu_irq *vpb_sic_init(uint32_t base, qemu_irq *parent, int irq)
{
    vpb_sic_state *s;
    qemu_irq *qi;
    int iomemtype;

    s = (vpb_sic_state *)qemu_mallocz(sizeof(vpb_sic_state));
    if (!s)
        return NULL;
    qi = qemu_allocate_irqs(vpb_sic_set_irq, s, 32);
    s->base = base;
    s->parent = parent;
    s->irq = irq;
    iomemtype = cpu_register_io_memory(0, vpb_sic_readfn,
                                       vpb_sic_writefn, s);
    cpu_register_physical_memory(base, 0x00000fff, iomemtype);
    /
    return qi;
}
