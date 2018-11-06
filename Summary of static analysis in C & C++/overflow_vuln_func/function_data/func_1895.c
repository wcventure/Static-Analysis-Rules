void *pl110_init(DisplayState *ds, uint32_t base, qemu_irq irq,
                 int versatile)
{
    pl110_state *s;
    int iomemtype;

    s = (pl110_state *)qemu_mallocz(sizeof(pl110_state));
    iomemtype = cpu_register_io_memory(0, pl110_readfn,
                                       pl110_writefn, s);
    cpu_register_physical_memory(base, 0x00000fff, iomemtype);
    s->base = base;
    s->ds = ds;
    s->versatile = versatile;
    s->irq = irq;
    graphic_console_init(ds, pl110_update_display, pl110_invalidate_display,
                         NULL, s);
    /
    return s;
}
