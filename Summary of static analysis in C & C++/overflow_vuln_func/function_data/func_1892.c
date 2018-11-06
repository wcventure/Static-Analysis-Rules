void pl011_init(uint32_t base, qemu_irq irq,
                CharDriverState *chr)
{
    int iomemtype;
    pl011_state *s;

    s = (pl011_state *)qemu_mallocz(sizeof(pl011_state));
    iomemtype = cpu_register_io_memory(0, pl011_readfn,
                                       pl011_writefn, s);
    cpu_register_physical_memory(base, 0x00000fff, iomemtype);
    s->base = base;
    s->irq = irq;
    s->chr = chr;
    s->read_trigger = 1;
    s->ifl = 0x12;
    s->cr = 0x300;
    s->flags = 0x90;
    if (chr){ 
        qemu_chr_add_handlers(chr, pl011_can_recieve, pl011_recieve,
                              pl011_event, s);
    }
    /
}
