static void stellaris_init(const char *kernel_filename, const char *cpu_model,
                           DisplayState *ds, stellaris_board_info *board)
{
    static const int uart_irq[] = {5, 6, 33, 34};
    static const int timer_irq[] = {19, 21, 23, 35};
    static const uint32_t gpio_addr[7] =
      { 0x40004000, 0x40005000, 0x40006000, 0x40007000,
        0x40024000, 0x40025000, 0x40026000};
    static const int gpio_irq[7] = {0, 1, 2, 3, 4, 30, 31};

    qemu_irq *pic;
    qemu_irq *gpio_in[5];
    qemu_irq *gpio_out[5];
    qemu_irq adc;
    int sram_size;
    int flash_size;
    i2c_bus *i2c;
    int i;

    flash_size = ((board->dc0 & 0xffff) + 1) << 1;
    sram_size = (board->dc0 >> 18) + 1;
    pic = armv7m_init(flash_size, sram_size, kernel_filename, cpu_model);

    if (board->dc1 & (1 << 16)) {
        adc = stellaris_adc_init(0x40038000, pic[14]);
    } else {
        adc = NULL;
    }
    for (i = 0; i < 4; i++) {
        if (board->dc2 & (0x10000 << i)) {
            stellaris_gptm_init(0x40030000 + i * 0x1000,
                                pic[timer_irq[i]], adc);
        }
    }

    stellaris_sys_init(0x400fe000, pic[28], board, nd_table[0].macaddr);

    for (i = 0; i < 7; i++) {
        if (board->dc4 & (1 << i)) {
            gpio_in[i] = pl061_init(gpio_addr[i], pic[gpio_irq[i]],
                                    &gpio_out[i]);
        }
    }

    if (board->dc2 & (1 << 12)) {
        i2c = i2c_init_bus();
        stellaris_i2c_init(0x40020000, pic[8], i2c);
        if (board->peripherals & BP_OLED_I2C) {
            ssd0303_init(ds, i2c, 0x3d);
        }
    }

    for (i = 0; i < 4; i++) {
        if (board->dc2 & (1 << i)) {
            pl011_init(0x4000c000 + i * 0x1000, pic[uart_irq[i]],
                       serial_hds[i], PL011_LUMINARY);
        }
    }
    if (board->dc2 & (1 << 4)) {
        if (board->peripherals & BP_OLED_SSI) {
            void * oled;
            void * sd;
            void *ssi_bus;
            int index;

            oled = ssd0323_init(ds, &gpio_out[GPIO_C][7]);
            index = drive_get_index(IF_SD, 0, 0);
            sd = ssi_sd_init(drives_table[index].bdrv);

            ssi_bus = stellaris_ssi_bus_init(&gpio_out[GPIO_D][0],
                                             ssi_sd_xfer, sd,
                                             ssd0323_xfer_ssi, oled);

            pl022_init(0x40008000, pic[7], stellaris_ssi_bus_xfer, ssi_bus);
            /
            qemu_irq_raise(gpio_out[GPIO_D][0]);
        } else {
            pl022_init(0x40008000, pic[7], NULL, NULL);
        }
    }
    if (board->dc4 & (1 << 28)) {
        /
        stellaris_enet_init(&nd_table[0], 0x40048000, pic[42]);
    }
    if (board->peripherals & BP_GAMEPAD) {
        qemu_irq gpad_irq[5];
        static const int gpad_keycode[5] = { 0xc8, 0xd0, 0xcb, 0xcd, 0x1d };

        gpad_irq[0] = qemu_irq_invert(gpio_in[GPIO_E][0]); /
        gpad_irq[1] = qemu_irq_invert(gpio_in[GPIO_E][1]); /
        gpad_irq[2] = qemu_irq_invert(gpio_in[GPIO_E][2]); /
        gpad_irq[3] = qemu_irq_invert(gpio_in[GPIO_E][3]); /
        gpad_irq[4] = qemu_irq_invert(gpio_in[GPIO_F][1]); /

        stellaris_gamepad_init(5, gpad_irq, gpad_keycode);
    }
}
