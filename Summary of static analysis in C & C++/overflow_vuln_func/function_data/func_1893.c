void pl050_init(uint32_t base, qemu_irq irq, int is_mouse)
{
    int iomemtype;
    pl050_state *s;

    s = (pl050_state *)qemu_mallocz(sizeof(pl050_state));
    iomemtype = cpu_register_io_memory(0, pl050_readfn,
                                       pl050_writefn, s);
    cpu_register_physical_memory(base, 0x00000fff, iomemtype);
    s->base = base;
    s->irq = irq;
    s->is_mouse = is_mouse;
    if (is_mouse)
        s->dev = ps2_mouse_init(pl050_update, s);
    else
        s->dev = ps2_kbd_init(pl050_update, s);
    /
}
