static void machvirt_init(MachineState *machine)
{
    VirtMachineState *vms = VIRT_MACHINE(machine);
    qemu_irq pic[NUM_IRQS];
    MemoryRegion *sysmem = get_system_memory();
    int n;
    MemoryRegion *ram = g_new(MemoryRegion, 1);
    const char *cpu_model = machine->cpu_model;
    VirtBoardInfo *vbi;

    if (!cpu_model) {
        cpu_model = "cortex-a15";
    }

    vbi = find_machine_info(cpu_model);

    if (!vbi) {
        error_report("mach-virt: CPU %s not supported", cpu_model);
        exit(1);
    }

    vbi->smp_cpus = smp_cpus;

    if (machine->ram_size > vbi->memmap[VIRT_MEM].size) {
        error_report("mach-virt: cannot model more than 30GB RAM");
        exit(1);
    }

    create_fdt(vbi);

    for (n = 0; n < smp_cpus; n++) {
        ObjectClass *oc = cpu_class_by_name(TYPE_ARM_CPU, cpu_model);
        Object *cpuobj;

        if (!oc) {
            fprintf(stderr, "Unable to find CPU definition\n");
            exit(1);
        }
        cpuobj = object_new(object_class_get_name(oc));

        if (!vms->secure) {
            object_property_set_bool(cpuobj, false, "has_el3", NULL);
        }

        object_property_set_int(cpuobj, QEMU_PSCI_CONDUIT_HVC, "psci-conduit",
                                NULL);

        /
        if (n > 0) {
            object_property_set_bool(cpuobj, true, "start-powered-off", NULL);
        }

        if (object_property_find(cpuobj, "reset-cbar", NULL)) {
            object_property_set_int(cpuobj, vbi->memmap[VIRT_CPUPERIPHS].base,
                                    "reset-cbar", &error_abort);
        }

        object_property_set_bool(cpuobj, true, "realized", NULL);
    }
    fdt_add_timer_nodes(vbi);
    fdt_add_cpu_nodes(vbi);
    fdt_add_psci_node(vbi);

    memory_region_init_ram(ram, NULL, "mach-virt.ram", machine->ram_size,
                           &error_abort);
    vmstate_register_ram_global(ram);
    memory_region_add_subregion(sysmem, vbi->memmap[VIRT_MEM].base, ram);

    create_flash(vbi);

    create_gic(vbi, pic);

    create_uart(vbi, pic);

    create_rtc(vbi, pic);

    /
    create_virtio_devices(vbi, pic);

    create_fw_cfg(vbi);

    vbi->bootinfo.ram_size = machine->ram_size;
    vbi->bootinfo.kernel_filename = machine->kernel_filename;
    vbi->bootinfo.kernel_cmdline = machine->kernel_cmdline;
    vbi->bootinfo.initrd_filename = machine->initrd_filename;
    vbi->bootinfo.nb_cpus = smp_cpus;
    vbi->bootinfo.board_id = -1;
    vbi->bootinfo.loader_start = vbi->memmap[VIRT_MEM].base;
    vbi->bootinfo.get_dtb = machvirt_dtb;
    vbi->bootinfo.firmware_loaded = bios_name || drive_get(IF_PFLASH, 0, 0);
    arm_load_kernel(ARM_CPU(first_cpu), &vbi->bootinfo);
}
