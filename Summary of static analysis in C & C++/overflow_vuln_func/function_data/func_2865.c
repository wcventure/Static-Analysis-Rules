static void numa_node_parse(NumaNodeOptions *node, QemuOpts *opts, Error **errp)
{
    uint16_t nodenr;
    uint16List *cpus = NULL;

    if (node->has_nodeid) {
        nodenr = node->nodeid;
    } else {
        nodenr = nb_numa_nodes;
    }

    if (nodenr >= MAX_NODES) {
        error_setg(errp, "Max number of NUMA nodes reached: %"
                   PRIu16 "", nodenr);
        return;
    }

    if (numa_info[nodenr].present) {
        error_setg(errp, "Duplicate NUMA nodeid: %" PRIu16, nodenr);
        return;
    }

    for (cpus = node->cpus; cpus; cpus = cpus->next) {
        if (cpus->value > MAX_CPUMASK_BITS) {
            error_setg(errp, "CPU number %" PRIu16 " is bigger than %d",
                       cpus->value, MAX_CPUMASK_BITS);
            return;
        }
        bitmap_set(numa_info[nodenr].node_cpu, cpus->value, 1);
    }

    if (node->has_mem && node->has_memdev) {
        error_setg(errp, "qemu: cannot specify both mem= and memdev=");
        return;
    }

    if (have_memdevs == -1) {
        have_memdevs = node->has_memdev;
    }
    if (node->has_memdev != have_memdevs) {
        error_setg(errp, "qemu: memdev option must be specified for either "
                   "all or no nodes");
        return;
    }

    if (node->has_mem) {
        uint64_t mem_size = node->mem;
        const char *mem_str = qemu_opt_get(opts, "mem");
        /
        if (g_ascii_isdigit(mem_str[strlen(mem_str) - 1])) {
            mem_size <<= 20;
        }
        numa_info[nodenr].node_mem = mem_size;
    }
    if (node->has_memdev) {
        Object *o;
        o = object_resolve_path_type(node->memdev, TYPE_MEMORY_BACKEND, NULL);
        if (!o) {
            error_setg(errp, "memdev=%s is ambiguous", node->memdev);
            return;
        }

        object_ref(o);
        numa_info[nodenr].node_mem = object_property_get_int(o, "size", NULL);
        numa_info[nodenr].node_memdev = MEMORY_BACKEND(o);
    }
    numa_info[nodenr].present = true;
    max_numa_nodeid = MAX(max_numa_nodeid, nodenr + 1);
}
