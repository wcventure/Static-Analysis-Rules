static void create_gic(const VirtBoardInfo *vbi, qemu_irq *pic)
{
    /
    DeviceState *gicdev;
    SysBusDevice *gicbusdev;
    const char *gictype = "arm_gic";
    int i;

    if (kvm_irqchip_in_kernel()) {
        gictype = "kvm-arm-gic";
    }

    gicdev = qdev_create(NULL, gictype);
    qdev_prop_set_uint32(gicdev, "revision", 2);
    qdev_prop_set_uint32(gicdev, "num-cpu", smp_cpus);
    /
    qdev_prop_set_uint32(gicdev, "num-irq", NUM_IRQS + 32);
    qdev_init_nofail(gicdev);
    gicbusdev = SYS_BUS_DEVICE(gicdev);
    sysbus_mmio_map(gicbusdev, 0, vbi->memmap[VIRT_GIC_DIST].base);
    sysbus_mmio_map(gicbusdev, 1, vbi->memmap[VIRT_GIC_CPU].base);

    /
    for (i = 0; i < smp_cpus; i++) {
        DeviceState *cpudev = DEVICE(qemu_get_cpu(i));
        int ppibase = NUM_IRQS + i * 32;
        /
        qdev_connect_gpio_out(cpudev, 0,
                              qdev_get_gpio_in(gicdev, ppibase + 30));
        /
        qdev_connect_gpio_out(cpudev, 1,
                              qdev_get_gpio_in(gicdev, ppibase + 27));

        sysbus_connect_irq(gicbusdev, i, qdev_get_gpio_in(cpudev, ARM_CPU_IRQ));
    }

    for (i = 0; i < NUM_IRQS; i++) {
        pic[i] = qdev_get_gpio_in(gicdev, i);
    }

    fdt_add_gic_node(vbi);
}
