static void sun4uv_init(MemoryRegion *address_space_mem,
                        MachineState *machine,
                        const struct hwdef *hwdef)
{
    SPARCCPU *cpu;
    M48t59State *nvram;
    unsigned int i;
    uint64_t initrd_addr, initrd_size, kernel_addr, kernel_size, kernel_entry;
    PCIBus *pci_bus, *pci_bus2, *pci_bus3;
    ISABus *isa_bus;
    qemu_irq *ivec_irqs, *pbm_irqs;
    DriveInfo *hd[MAX_IDE_BUS * MAX_IDE_DEVS];
    DriveInfo *fd[MAX_FD];
    FWCfgState *fw_cfg;

    /
    cpu = cpu_devinit(machine->cpu_model, hwdef);

    /
    ram_init(0, machine->ram_size);

    prom_init(hwdef->prom_addr, bios_name);

    ivec_irqs = qemu_allocate_irqs(cpu_set_ivec_irq, cpu, IVEC_MAX);
    pci_bus = pci_apb_init(APB_SPECIAL_BASE, APB_MEM_BASE, ivec_irqs, &pci_bus2,
                           &pci_bus3, &pbm_irqs);
    pci_vga_init(pci_bus);

    // XXX Should be pci_bus3
    isa_bus = pci_ebus_init(pci_bus, -1, pbm_irqs);

    i = 0;
    if (hwdef->console_serial_base) {
        serial_mm_init(address_space_mem, hwdef->console_serial_base, 0,
                       NULL, 115200, serial_hds[i], DEVICE_BIG_ENDIAN);
        i++;
    }
    for(; i < MAX_SERIAL_PORTS; i++) {
        if (serial_hds[i]) {
            serial_isa_init(isa_bus, i, serial_hds[i]);
        }
    }

    for(i = 0; i < MAX_PARALLEL_PORTS; i++) {
        if (parallel_hds[i]) {
            parallel_init(isa_bus, i, parallel_hds[i]);
        }
    }

    for(i = 0; i < nb_nics; i++)
        pci_nic_init_nofail(&nd_table[i], pci_bus, "ne2k_pci", NULL);

    ide_drive_get(hd, MAX_IDE_BUS);

    pci_cmd646_ide_init(pci_bus, hd, 1);

    isa_create_simple(isa_bus, "i8042");
    for(i = 0; i < MAX_FD; i++) {
        fd[i] = drive_get(IF_FLOPPY, 0, i);
    }
    fdctrl_init_isa(isa_bus, fd);
    nvram = m48t59_init_isa(isa_bus, 0x0074, NVRAM_SIZE, 59);

    initrd_size = 0;
    initrd_addr = 0;
    kernel_size = sun4u_load_kernel(machine->kernel_filename,
                                    machine->initrd_filename,
                                    ram_size, &initrd_size, &initrd_addr,
                                    &kernel_addr, &kernel_entry);

    sun4u_NVRAM_set_params(nvram, NVRAM_SIZE, "Sun4u", machine->ram_size,
                           machine->boot_order,
                           kernel_addr, kernel_size,
                           machine->kernel_cmdline,
                           initrd_addr, initrd_size,
                           /
                           0,
                           graphic_width, graphic_height, graphic_depth,
                           (uint8_t *)&nd_table[0].macaddr);

    fw_cfg = fw_cfg_init(BIOS_CFG_IOPORT, BIOS_CFG_IOPORT + 1, 0, 0);
    fw_cfg_add_i16(fw_cfg, FW_CFG_MAX_CPUS, (uint16_t)max_cpus);
    fw_cfg_add_i32(fw_cfg, FW_CFG_ID, 1);
    fw_cfg_add_i64(fw_cfg, FW_CFG_RAM_SIZE, (uint64_t)ram_size);
    fw_cfg_add_i16(fw_cfg, FW_CFG_MACHINE_ID, hwdef->machine_id);
    fw_cfg_add_i64(fw_cfg, FW_CFG_KERNEL_ADDR, kernel_entry);
    fw_cfg_add_i64(fw_cfg, FW_CFG_KERNEL_SIZE, kernel_size);
    if (machine->kernel_cmdline) {
        fw_cfg_add_i32(fw_cfg, FW_CFG_CMDLINE_SIZE,
                       strlen(machine->kernel_cmdline) + 1);
        fw_cfg_add_string(fw_cfg, FW_CFG_CMDLINE_DATA, machine->kernel_cmdline);
    } else {
        fw_cfg_add_i32(fw_cfg, FW_CFG_CMDLINE_SIZE, 0);
    }
    fw_cfg_add_i64(fw_cfg, FW_CFG_INITRD_ADDR, initrd_addr);
    fw_cfg_add_i64(fw_cfg, FW_CFG_INITRD_SIZE, initrd_size);
    fw_cfg_add_i16(fw_cfg, FW_CFG_BOOT_DEVICE, machine->boot_order[0]);

    fw_cfg_add_i16(fw_cfg, FW_CFG_SPARC64_WIDTH, graphic_width);
    fw_cfg_add_i16(fw_cfg, FW_CFG_SPARC64_HEIGHT, graphic_height);
    fw_cfg_add_i16(fw_cfg, FW_CFG_SPARC64_DEPTH, graphic_depth);

    qemu_register_boot_set(fw_cfg_boot_set, fw_cfg);
}
