struct pxa2xx_state_s *pxa255_init(unsigned int sdram_size,
                DisplayState *ds)
{
    struct pxa2xx_state_s *s;
    struct pxa2xx_ssp_s *ssp;
    int iomemtype, i;
    s = (struct pxa2xx_state_s *) qemu_mallocz(sizeof(struct pxa2xx_state_s));

    s->env = cpu_init();
    cpu_arm_set_model(s->env, "pxa255");
    register_savevm("cpu", 0, 0, cpu_save, cpu_load, s->env);

    /
    cpu_register_physical_memory(PXA2XX_SDRAM_BASE, sdram_size,
                    qemu_ram_alloc(sdram_size) | IO_MEM_RAM);
    cpu_register_physical_memory(PXA2XX_INTERNAL_BASE, PXA2XX_INTERNAL_SIZE,
                    qemu_ram_alloc(PXA2XX_INTERNAL_SIZE) | IO_MEM_RAM);

    s->pic = pxa2xx_pic_init(0x40d00000, s->env);

    s->dma = pxa255_dma_init(0x40000000, s->pic[PXA2XX_PIC_DMA]);

    pxa25x_timer_init(0x40a00000, &s->pic[PXA2XX_PIC_OST_0]);

    s->gpio = pxa2xx_gpio_init(0x40e00000, s->env, s->pic, 85);

    s->mmc = pxa2xx_mmci_init(0x41100000, s->pic[PXA2XX_PIC_MMC], s->dma);

    for (i = 0; pxa255_serial[i].io_base; i ++)
        if (serial_hds[i])
            serial_mm_init(pxa255_serial[i].io_base, 2,
                            s->pic[pxa255_serial[i].irqn], serial_hds[i], 1);
        else
            break;
    if (serial_hds[i])
        s->fir = pxa2xx_fir_init(0x40800000, s->pic[PXA2XX_PIC_ICP],
                        s->dma, serial_hds[i]);

    if (ds)
        s->lcd = pxa2xx_lcdc_init(0x44000000, s->pic[PXA2XX_PIC_LCD], ds);

    s->cm_base = 0x41300000;
    s->cm_regs[CCCR >> 4] = 0x02000210;	/
    s->clkcfg = 0x00000009;		/
    iomemtype = cpu_register_io_memory(0, pxa2xx_cm_readfn,
                    pxa2xx_cm_writefn, s);
    cpu_register_physical_memory(s->cm_base, 0xfff, iomemtype);
    register_savevm("pxa2xx_cm", 0, 0, pxa2xx_cm_save, pxa2xx_cm_load, s);

    cpu_arm_set_cp_io(s->env, 14, pxa2xx_cp14_read, pxa2xx_cp14_write, s);

    s->mm_base = 0x48000000;
    s->mm_regs[MDMRS >> 2] = 0x00020002;
    s->mm_regs[MDREFR >> 2] = 0x03ca4000;
    s->mm_regs[MECR >> 2] = 0x00000001;	/
    iomemtype = cpu_register_io_memory(0, pxa2xx_mm_readfn,
                    pxa2xx_mm_writefn, s);
    cpu_register_physical_memory(s->mm_base, 0xfff, iomemtype);
    register_savevm("pxa2xx_mm", 0, 0, pxa2xx_mm_save, pxa2xx_mm_load, s);

    s->pm_base = 0x40f00000;
    iomemtype = cpu_register_io_memory(0, pxa2xx_pm_readfn,
                    pxa2xx_pm_writefn, s);
    cpu_register_physical_memory(s->pm_base, 0xff, iomemtype);
    register_savevm("pxa2xx_pm", 0, 0, pxa2xx_pm_save, pxa2xx_pm_load, s);

    for (i = 0; pxa255_ssp[i].io_base; i ++);
    s->ssp = (struct pxa2xx_ssp_s **)
            qemu_mallocz(sizeof(struct pxa2xx_ssp_s *) * i);
    ssp = (struct pxa2xx_ssp_s *)
            qemu_mallocz(sizeof(struct pxa2xx_ssp_s) * i);
    for (i = 0; pxa255_ssp[i].io_base; i ++) {
        s->ssp[i] = &ssp[i];
        ssp[i].base = pxa255_ssp[i].io_base;
        ssp[i].irq = s->pic[pxa255_ssp[i].irqn];

        iomemtype = cpu_register_io_memory(0, pxa2xx_ssp_readfn,
                        pxa2xx_ssp_writefn, &ssp[i]);
        cpu_register_physical_memory(ssp[i].base, 0xfff, iomemtype);
        register_savevm("pxa2xx_ssp", i, 0,
                        pxa2xx_ssp_save, pxa2xx_ssp_load, s);
    }

    if (usb_enabled) {
        usb_ohci_init_pxa(0x4c000000, 3, -1, s->pic[PXA2XX_PIC_USBH1]);
    }

    s->pcmcia[0] = pxa2xx_pcmcia_init(0x20000000);
    s->pcmcia[1] = pxa2xx_pcmcia_init(0x30000000);

    s->rtc_base = 0x40900000;
    iomemtype = cpu_register_io_memory(0, pxa2xx_rtc_readfn,
                    pxa2xx_rtc_writefn, s);
    cpu_register_physical_memory(s->rtc_base, 0xfff, iomemtype);
    pxa2xx_rtc_init(s);
    register_savevm("pxa2xx_rtc", 0, 0, pxa2xx_rtc_save, pxa2xx_rtc_load, s);

    s->i2c[0] = pxa2xx_i2c_init(0x40301600, s->pic[PXA2XX_PIC_I2C], 0xffff);
    s->i2c[1] = pxa2xx_i2c_init(0x40f00100, s->pic[PXA2XX_PIC_PWRI2C], 0xff);

    s->i2s = pxa2xx_i2s_init(0x40400000, s->pic[PXA2XX_PIC_I2S], s->dma);

    /
    /
    pxa2xx_gpio_handler_set(s->gpio, 1, pxa2xx_reset, s);
    return s;
}
