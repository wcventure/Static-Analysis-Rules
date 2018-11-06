static void xlnx_ep108_init(MachineState *machine)
{
    XlnxEP108 *s = g_new0(XlnxEP108, 1);
    Error *err = NULL;

    object_initialize(&s->soc, sizeof(s->soc), TYPE_XLNX_ZYNQMP);
    object_property_add_child(OBJECT(machine), "soc", OBJECT(&s->soc),
                              &error_abort);

    object_property_set_bool(OBJECT(&s->soc), true, "realized", &err);
    if (err) {
        error_report("%s", error_get_pretty(err));
        exit(1);
    }

    if (machine->ram_size > EP108_MAX_RAM_SIZE) {
        error_report("WARNING: RAM size " RAM_ADDR_FMT " above max supported, "
                     "reduced to %llx", machine->ram_size, EP108_MAX_RAM_SIZE);
        machine->ram_size = EP108_MAX_RAM_SIZE;
    }

    if (machine->ram_size <= 0x08000000) {
        qemu_log("WARNING: RAM size " RAM_ADDR_FMT " is small for EP108",
                 machine->ram_size);
    }

    memory_region_allocate_system_memory(&s->ddr_ram, NULL, "ddr-ram",
                                         machine->ram_size);
    memory_region_add_subregion(get_system_memory(), 0, &s->ddr_ram);

    xlnx_ep108_binfo.ram_size = machine->ram_size;
    xlnx_ep108_binfo.kernel_filename = machine->kernel_filename;
    xlnx_ep108_binfo.kernel_cmdline = machine->kernel_cmdline;
    xlnx_ep108_binfo.initrd_filename = machine->initrd_filename;
    xlnx_ep108_binfo.loader_start = 0;
    arm_load_kernel(s->soc.boot_cpu_ptr, &xlnx_ep108_binfo);
}
