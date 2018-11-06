static void spapr_add_lmbs(DeviceState *dev, uint64_t addr, uint64_t size,
                           uint32_t node, Error **errp)
{
    sPAPRDRConnector *drc;
    sPAPRDRConnectorClass *drck;
    uint32_t nr_lmbs = size/SPAPR_MEMORY_BLOCK_SIZE;
    int i, fdt_offset, fdt_size;
    void *fdt;

    /
    if (!dev->hotplugged) {
        return;
    }

    for (i = 0; i < nr_lmbs; i++) {
        drc = spapr_dr_connector_by_id(SPAPR_DR_CONNECTOR_TYPE_LMB,
                addr/SPAPR_MEMORY_BLOCK_SIZE);
        g_assert(drc);

        fdt = create_device_tree(&fdt_size);
        fdt_offset = spapr_populate_memory_node(fdt, node, addr,
                                                SPAPR_MEMORY_BLOCK_SIZE);

        drck = SPAPR_DR_CONNECTOR_GET_CLASS(drc);
        drck->attach(drc, dev, fdt, fdt_offset, !dev->hotplugged, errp);
        spapr_hotplug_req_add_by_index(drc);
        addr += SPAPR_MEMORY_BLOCK_SIZE;
    }
}
