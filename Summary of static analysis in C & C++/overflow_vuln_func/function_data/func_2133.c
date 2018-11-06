static void xilinx_spips_realize(DeviceState *dev, Error **errp)
{
    XilinxSPIPS *s = XILINX_SPIPS(dev);
    SysBusDevice *sbd = SYS_BUS_DEVICE(dev);
    XilinxSPIPSClass *xsc = XILINX_SPIPS_GET_CLASS(s);
    int i;

    DB_PRINT_L(0, "realized spips\n");

    s->spi = g_new(SSIBus *, s->num_busses);
    for (i = 0; i < s->num_busses; ++i) {
        char bus_name[16];
        snprintf(bus_name, 16, "spi%d", i);
        s->spi[i] = ssi_create_bus(dev, bus_name);
    }

    s->cs_lines = g_new0(qemu_irq, s->num_cs * s->num_busses);
    ssi_auto_connect_slaves(DEVICE(s), s->cs_lines, s->spi[0]);
    ssi_auto_connect_slaves(DEVICE(s), s->cs_lines, s->spi[1]);
    sysbus_init_irq(sbd, &s->irq);
    for (i = 0; i < s->num_cs * s->num_busses; ++i) {
        sysbus_init_irq(sbd, &s->cs_lines[i]);
    }

    memory_region_init_io(&s->iomem, OBJECT(s), xsc->reg_ops, s,
                          "spi", XLNX_SPIPS_R_MAX * 4);
    sysbus_init_mmio(sbd, &s->iomem);

    s->irqline = -1;

    fifo8_create(&s->rx_fifo, xsc->rx_fifo_size);
    fifo8_create(&s->tx_fifo, xsc->tx_fifo_size);
}
