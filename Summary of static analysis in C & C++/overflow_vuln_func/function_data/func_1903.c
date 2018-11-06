struct pxa2xx_lcdc_s *pxa2xx_lcdc_init(target_phys_addr_t base, qemu_irq irq,
                DisplayState *ds)
{
    int iomemtype;
    struct pxa2xx_lcdc_s *s;

    s = (struct pxa2xx_lcdc_s *) qemu_mallocz(sizeof(struct pxa2xx_lcdc_s));
    s->base = base;
    s->invalidated = 1;
    s->irq = irq;
    s->ds = ds;

    pxa2xx_lcdc_orientation(s, graphic_rotate);

    iomemtype = cpu_register_io_memory(0, pxa2xx_lcdc_readfn,
                    pxa2xx_lcdc_writefn, s);
    cpu_register_physical_memory(base, 0x000fffff, iomemtype);

    graphic_console_init(ds, pxa2xx_update_display,
                    pxa2xx_invalidate_display, pxa2xx_screen_dump, s);

    switch (s->ds->depth) {
    case 0:
        s->dest_width = 0;
        break;
    case 8:
        s->line_fn[0] = pxa2xx_draw_fn_8;
        s->line_fn[1] = pxa2xx_draw_fn_8t;
        s->dest_width = 1;
        break;
    case 15:
        s->line_fn[0] = pxa2xx_draw_fn_15;
        s->line_fn[1] = pxa2xx_draw_fn_15t;
        s->dest_width = 2;
        break;
    case 16:
        s->line_fn[0] = pxa2xx_draw_fn_16;
        s->line_fn[1] = pxa2xx_draw_fn_16t;
        s->dest_width = 2;
        break;
    case 24:
        s->line_fn[0] = pxa2xx_draw_fn_24;
        s->line_fn[1] = pxa2xx_draw_fn_24t;
        s->dest_width = 3;
        break;
    case 32:
        s->line_fn[0] = pxa2xx_draw_fn_32;
        s->line_fn[1] = pxa2xx_draw_fn_32t;
        s->dest_width = 4;
        break;
    default:
        fprintf(stderr, "%s: Bad color depth\n", __FUNCTION__);
        exit(1);
    }

    register_savevm("pxa2xx_lcdc", 0, 0,
                    pxa2xx_lcdc_save, pxa2xx_lcdc_load, s);

    return s;
}
