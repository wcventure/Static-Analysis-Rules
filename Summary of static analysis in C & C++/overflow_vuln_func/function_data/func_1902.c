struct pxa2xx_gpio_info_s *pxa2xx_gpio_init(target_phys_addr_t base,
                CPUState *env, qemu_irq *pic, int lines)
{
    int iomemtype;
    struct pxa2xx_gpio_info_s *s;

    s = (struct pxa2xx_gpio_info_s *)
            qemu_mallocz(sizeof(struct pxa2xx_gpio_info_s));
    memset(s, 0, sizeof(struct pxa2xx_gpio_info_s));
    s->base = base;
    s->pic = pic;
    s->lines = lines;
    s->cpu_env = env;

    iomemtype = cpu_register_io_memory(0, pxa2xx_gpio_readfn,
                    pxa2xx_gpio_writefn, s);
    cpu_register_physical_memory(base, 0x00000fff, iomemtype);

    register_savevm("pxa2xx_gpio", 0, 0,
                    pxa2xx_gpio_save, pxa2xx_gpio_load, s);

    return s;
}
