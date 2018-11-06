static void ccw_machine_class_init(ObjectClass *oc, void *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);
    NMIClass *nc = NMI_CLASS(oc);
    HotplugHandlerClass *hc = HOTPLUG_HANDLER_CLASS(oc);
    S390CcwMachineClass *s390mc = S390_MACHINE_CLASS(mc);

    s390mc->ri_allowed = true;
    mc->init = ccw_init;
    mc->reset = s390_machine_reset;
    mc->hot_add_cpu = s390_hot_add_cpu;
    mc->block_default_type = IF_VIRTIO;
    mc->no_cdrom = 1;
    mc->no_floppy = 1;
    mc->no_serial = 1;
    mc->no_parallel = 1;
    mc->no_sdcard = 1;
    mc->use_sclp = 1;
    mc->max_cpus = 255;
    mc->get_hotplug_handler = s390_get_hotplug_handler;
    hc->plug = s390_machine_device_plug;
    nc->nmi_monitor_handler = s390_nmi;
}
