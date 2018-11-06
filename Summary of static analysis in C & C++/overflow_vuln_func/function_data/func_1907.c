static pxa2xx_timer_info *pxa2xx_timer_init(target_phys_addr_t base,
                qemu_irq *irqs)
{
    int i;
    int iomemtype;
    pxa2xx_timer_info *s;

    s = (pxa2xx_timer_info *) qemu_mallocz(sizeof(pxa2xx_timer_info));
    s->base = base;
    s->irq_enabled = 0;
    s->oldclock = 0;
    s->clock = 0;
    s->lastload = qemu_get_clock(vm_clock);
    s->reset3 = 0;

    for (i = 0; i < 4; i ++) {
        s->timer[i].value = 0;
        s->timer[i].irq = irqs[i];
        s->timer[i].info = s;
        s->timer[i].num = i;
        s->timer[i].level = 0;
        s->timer[i].qtimer = qemu_new_timer(vm_clock,
                        pxa2xx_timer_tick, &s->timer[i]);
    }

    iomemtype = cpu_register_io_memory(0, pxa2xx_timer_readfn,
                    pxa2xx_timer_writefn, s);
    cpu_register_physical_memory(base, 0x00000fff, iomemtype);

    register_savevm("pxa2xx_timer", 0, 0,
                    pxa2xx_timer_save, pxa2xx_timer_load, s);

    return s;
}
