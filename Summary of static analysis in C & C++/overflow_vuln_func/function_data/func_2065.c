static int spapr_populate_drconf_memory(sPAPRMachineState *spapr, void *fdt)
{
    MachineState *machine = MACHINE(spapr);
    int ret, i, offset;
    uint64_t lmb_size = SPAPR_MEMORY_BLOCK_SIZE;
    uint32_t prop_lmb_size[] = {0, cpu_to_be32(lmb_size)};
    uint32_t nr_lmbs = (machine->maxram_size - machine->ram_size)/lmb_size;
    uint32_t *int_buf, *cur_index, buf_len;
    int nr_nodes = nb_numa_nodes ? nb_numa_nodes : 1;

    /
    buf_len = nr_lmbs * SPAPR_DR_LMB_LIST_ENTRY_SIZE * sizeof(uint32_t) +
                sizeof(uint32_t);
    cur_index = int_buf = g_malloc0(buf_len);

    offset = fdt_add_subnode(fdt, 0, "ibm,dynamic-reconfiguration-memory");

    ret = fdt_setprop(fdt, offset, "ibm,lmb-size", prop_lmb_size,
                    sizeof(prop_lmb_size));
    if (ret < 0) {
        goto out;
    }

    ret = fdt_setprop_cell(fdt, offset, "ibm,memory-flags-mask", 0xff);
    if (ret < 0) {
        goto out;
    }

    ret = fdt_setprop_cell(fdt, offset, "ibm,memory-preservation-time", 0x0);
    if (ret < 0) {
        goto out;
    }

    /
    int_buf[0] = cpu_to_be32(nr_lmbs);
    cur_index++;
    for (i = 0; i < nr_lmbs; i++) {
        sPAPRDRConnector *drc;
        sPAPRDRConnectorClass *drck;
        uint64_t addr = i * lmb_size + spapr->hotplug_memory.base;;
        uint32_t *dynamic_memory = cur_index;

        drc = spapr_dr_connector_by_id(SPAPR_DR_CONNECTOR_TYPE_LMB,
                                       addr/lmb_size);
        g_assert(drc);
        drck = SPAPR_DR_CONNECTOR_GET_CLASS(drc);

        dynamic_memory[0] = cpu_to_be32(addr >> 32);
        dynamic_memory[1] = cpu_to_be32(addr & 0xffffffff);
        dynamic_memory[2] = cpu_to_be32(drck->get_index(drc));
        dynamic_memory[3] = cpu_to_be32(0); /
        dynamic_memory[4] = cpu_to_be32(numa_get_node(addr, NULL));
        if (addr < machine->ram_size ||
                    memory_region_present(get_system_memory(), addr)) {
            dynamic_memory[5] = cpu_to_be32(SPAPR_LMB_FLAGS_ASSIGNED);
        } else {
            dynamic_memory[5] = cpu_to_be32(0);
        }

        cur_index += SPAPR_DR_LMB_LIST_ENTRY_SIZE;
    }
    ret = fdt_setprop(fdt, offset, "ibm,dynamic-memory", int_buf, buf_len);
    if (ret < 0) {
        goto out;
    }

    /
    cur_index = int_buf;
    int_buf[0] = cpu_to_be32(nr_nodes);
    int_buf[1] = cpu_to_be32(4); /
    cur_index += 2;
    for (i = 0; i < nr_nodes; i++) {
        uint32_t associativity[] = {
            cpu_to_be32(0x0),
            cpu_to_be32(0x0),
            cpu_to_be32(0x0),
            cpu_to_be32(i)
        };
        memcpy(cur_index, associativity, sizeof(associativity));
        cur_index += 4;
    }
    ret = fdt_setprop(fdt, offset, "ibm,associativity-lookup-arrays", int_buf,
            (cur_index - int_buf) * sizeof(uint32_t));
out:
    g_free(int_buf);
    return ret;
}
