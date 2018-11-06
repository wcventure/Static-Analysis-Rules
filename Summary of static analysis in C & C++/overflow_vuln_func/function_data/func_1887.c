void sp804_init(uint32_t base, qemu_irq irq)
{
    int iomemtype;
    sp804_state *s;
    qemu_irq *qi;

    s = (sp804_state *)qemu_mallocz(sizeof(sp804_state));
    qi = qemu_allocate_irqs(sp804_set_irq, s, 2);
    s->base = base;
    s->irq = irq;
    /
    s->timer[0] = arm_timer_init(1000000, qi[0]);
    s->timer[1] = arm_timer_init(1000000, qi[1]);
    iomemtype = cpu_register_io_memory(0, sp804_readfn,
                                       sp804_writefn, s);
    cpu_register_physical_memory(base, 0x00000fff, iomemtype);
    /
}
