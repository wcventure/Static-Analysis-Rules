static void spapr_memory_plug(HotplugHandler *hotplug_dev, DeviceState *dev,
                              uint32_t node, Error **errp)
{
    Error *local_err = NULL;
    sPAPRMachineState *ms = SPAPR_MACHINE(hotplug_dev);
    PCDIMMDevice *dimm = PC_DIMM(dev);
    PCDIMMDeviceClass *ddc = PC_DIMM_GET_CLASS(dimm);
    MemoryRegion *mr = ddc->get_memory_region(dimm);
    uint64_t align = memory_region_get_alignment(mr);
    uint64_t size = memory_region_size(mr);
    uint64_t addr;

    if (size % SPAPR_MEMORY_BLOCK_SIZE) {
        error_setg(&local_err, "Hotplugged memory size must be a multiple of "
                      "%lld MB", SPAPR_MEMORY_BLOCK_SIZE/M_BYTE);
        goto out;
    }

    pc_dimm_memory_plug(dev, &ms->hotplug_memory, mr, align, &local_err);
    if (local_err) {
        goto out;
    }

    addr = object_property_get_int(OBJECT(dimm), PC_DIMM_ADDR_PROP, &local_err);
    if (local_err) {
        pc_dimm_memory_unplug(dev, &ms->hotplug_memory, mr);
        goto out;
    }

    spapr_add_lmbs(dev, addr, size, node, &error_abort);

out:
    error_propagate(errp, local_err);
}
