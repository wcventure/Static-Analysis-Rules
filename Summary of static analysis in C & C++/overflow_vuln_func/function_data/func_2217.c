static void clipper_init(MachineState *machine)
{
    ram_addr_t ram_size = machine->ram_size;
    const char *cpu_model = machine->cpu_model;
    const char *kernel_filename = machine->kernel_filename;
    const char *kernel_cmdline = machine->kernel_cmdline;
    const char *initrd_filename = machine->initrd_filename;
    AlphaCPU *cpus[4];
    PCIBus *pci_bus;
    ISABus *isa_bus;
    qemu_irq rtc_irq;
    long size, i;
    const char *palcode_filename;
    uint64_t palcode_entry, palcode_low, palcode_high;
    uint64_t kernel_entry, kernel_low, kernel_high;

    /
    memset(cpus, 0, sizeof(cpus));
    for (i = 0; i < smp_cpus; ++i) {
        cpus[i] = cpu_alpha_init(cpu_model ? cpu_model : "ev67");
    }

    cpus[0]->env.trap_arg0 = ram_size;
    cpus[0]->env.trap_arg1 = 0;
    cpus[0]->env.trap_arg2 = smp_cpus;

    /
    pci_bus = typhoon_init(ram_size, &isa_bus, &rtc_irq, cpus,
                           clipper_pci_map_irq);

    /
    rtc_init(isa_bus, 1900, rtc_irq);

    pit_init(isa_bus, 0x40, 0, NULL);
    isa_create_simple(isa_bus, "i8042");

    /
    pci_vga_init(pci_bus);

    /
    for (i = 0; i < MAX_SERIAL_PORTS; ++i) {
        if (serial_hds[i]) {
            serial_isa_init(isa_bus, i, serial_hds[i]);
        }
    }

    /
    for (i = 0; i < nb_nics; i++) {
        pci_nic_init_nofail(&nd_table[i], pci_bus, "e1000", NULL);
    }

    /
    {
        DriveInfo *hd[MAX_IDE_BUS * MAX_IDE_DEVS];
        ide_drive_get(hd, MAX_IDE_BUS);

        pci_cmd646_ide_init(pci_bus, hd, 0);
    }

    /
    palcode_filename = (bios_name ? bios_name : "palcode-clipper");
    palcode_filename = qemu_find_file(QEMU_FILE_TYPE_BIOS, palcode_filename);
    if (palcode_filename == NULL) {
        hw_error("no palcode provided\n");
        exit(1);
    }
    size = load_elf(palcode_filename, cpu_alpha_superpage_to_phys,
                    NULL, &palcode_entry, &palcode_low, &palcode_high,
                    0, EM_ALPHA, 0);
    if (size < 0) {
        hw_error("could not load palcode '%s'\n", palcode_filename);
        exit(1);
    }

    /
    for (i = 0; i < smp_cpus; ++i) {
        cpus[i]->env.pal_mode = 1;
        cpus[i]->env.pc = palcode_entry;
        cpus[i]->env.palbr = palcode_entry;
    }

    /
    if (kernel_filename) {
        uint64_t param_offset;

        size = load_elf(kernel_filename, cpu_alpha_superpage_to_phys,
                        NULL, &kernel_entry, &kernel_low, &kernel_high,
                        0, EM_ALPHA, 0);
        if (size < 0) {
            hw_error("could not load kernel '%s'\n", kernel_filename);
            exit(1);
        }

        cpus[0]->env.trap_arg1 = kernel_entry;

        param_offset = kernel_low - 0x6000;

        if (kernel_cmdline) {
            pstrcpy_targphys("cmdline", param_offset, 0x100, kernel_cmdline);
        }

        if (initrd_filename) {
            long initrd_base, initrd_size;

            initrd_size = get_image_size(initrd_filename);
            if (initrd_size < 0) {
                hw_error("could not load initial ram disk '%s'\n",
                         initrd_filename);
                exit(1);
            }

            /
            initrd_base = (ram_size - initrd_size) & TARGET_PAGE_MASK;
            load_image_targphys(initrd_filename, initrd_base,
                                ram_size - initrd_base);

            stq_phys(&address_space_memory,
                     param_offset + 0x100, initrd_base + 0xfffffc0000000000ULL);
            stq_phys(&address_space_memory, param_offset + 0x108, initrd_size);
        }
    }
}
