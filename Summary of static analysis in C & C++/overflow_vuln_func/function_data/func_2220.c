static void mips_fulong2e_init(MachineState *machine)
{
    ram_addr_t ram_size = machine->ram_size;
    const char *cpu_model = machine->cpu_model;
    const char *kernel_filename = machine->kernel_filename;
    const char *kernel_cmdline = machine->kernel_cmdline;
    const char *initrd_filename = machine->initrd_filename;
    char *filename;
    MemoryRegion *address_space_mem = get_system_memory();
    MemoryRegion *ram = g_new(MemoryRegion, 1);
    MemoryRegion *bios = g_new(MemoryRegion, 1);
    long bios_size;
    int64_t kernel_entry;
    qemu_irq *i8259;
    qemu_irq *cpu_exit_irq;
    PCIBus *pci_bus;
    ISABus *isa_bus;
    I2CBus *smbus;
    int i;
    DriveInfo *hd[MAX_IDE_BUS * MAX_IDE_DEVS];
    MIPSCPU *cpu;
    CPUMIPSState *env;

    /
    if (cpu_model == NULL) {
        cpu_model = "Loongson-2E";
    }
    cpu = cpu_mips_init(cpu_model);
    if (cpu == NULL) {
        fprintf(stderr, "Unable to find CPU definition\n");
        exit(1);
    }
    env = &cpu->env;

    qemu_register_reset(main_cpu_reset, cpu);

    /
    ram_size = 256 * 1024 * 1024;

    /
    bios_size = 1024 * 1024;

    /
    memory_region_init_ram(ram, NULL, "fulong2e.ram", ram_size, &error_abort);
    vmstate_register_ram_global(ram);
    memory_region_init_ram(bios, NULL, "fulong2e.bios", bios_size,
                           &error_abort);
    vmstate_register_ram_global(bios);
    memory_region_set_readonly(bios, true);

    memory_region_add_subregion(address_space_mem, 0, ram);
    memory_region_add_subregion(address_space_mem, 0x1fc00000LL, bios);

    /

    if (kernel_filename) {
        loaderparams.ram_size = ram_size;
        loaderparams.kernel_filename = kernel_filename;
        loaderparams.kernel_cmdline = kernel_cmdline;
        loaderparams.initrd_filename = initrd_filename;
        kernel_entry = load_kernel (env);
        write_bootloader(env, memory_region_get_ram_ptr(bios), kernel_entry);
    } else {
        if (bios_name == NULL) {
                bios_name = FULONG_BIOSNAME;
        }
        filename = qemu_find_file(QEMU_FILE_TYPE_BIOS, bios_name);
        if (filename) {
            bios_size = load_image_targphys(filename, 0x1fc00000LL,
                                            BIOS_SIZE);
            g_free(filename);
        } else {
            bios_size = -1;
        }

        if ((bios_size < 0 || bios_size > BIOS_SIZE) &&
            !kernel_filename && !qtest_enabled()) {
            error_report("Could not load MIPS bios '%s'", bios_name);
            exit(1);
        }
    }

    /
    cpu_mips_irq_init_cpu(env);
    cpu_mips_clock_init(env);

    /
    pci_bus = bonito_init((qemu_irq *)&(env->irq[2]));

    /
    ide_drive_get(hd, MAX_IDE_BUS);

    isa_bus = vt82c686b_init(pci_bus, PCI_DEVFN(FULONG2E_VIA_SLOT, 0));
    if (!isa_bus) {
        fprintf(stderr, "vt82c686b_init error\n");
        exit(1);
    }

    /
    /
    i8259 = i8259_init(isa_bus, env->irq[5]);
    isa_bus_irqs(isa_bus, i8259);

    vt82c686b_ide_init(pci_bus, hd, PCI_DEVFN(FULONG2E_VIA_SLOT, 1));
    pci_create_simple(pci_bus, PCI_DEVFN(FULONG2E_VIA_SLOT, 2),
                      "vt82c686b-usb-uhci");
    pci_create_simple(pci_bus, PCI_DEVFN(FULONG2E_VIA_SLOT, 3),
                      "vt82c686b-usb-uhci");

    smbus = vt82c686b_pm_init(pci_bus, PCI_DEVFN(FULONG2E_VIA_SLOT, 4),
                              0xeee1, NULL);
    /
    smbus_eeprom_init(smbus, 1, eeprom_spd, sizeof(eeprom_spd));

    /
    pit = pit_init(isa_bus, 0x40, 0, NULL);
    cpu_exit_irq = qemu_allocate_irqs(cpu_request_exit, NULL, 1);
    DMA_init(0, cpu_exit_irq);

    /
    isa_create_simple(isa_bus, "i8042");

    rtc_init(isa_bus, 2000, NULL);

    for(i = 0; i < MAX_SERIAL_PORTS; i++) {
        if (serial_hds[i]) {
            serial_isa_init(isa_bus, i, serial_hds[i]);
        }
    }

    if (parallel_hds[0]) {
        parallel_init(isa_bus, 0, parallel_hds[0]);
    }

    /
    audio_init(pci_bus);
    /
    network_init(pci_bus);
}
