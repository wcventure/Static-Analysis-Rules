static void numa_add(const char *optarg)
{
    char option[128];
    char *endptr;
    unsigned long long value, endvalue;
    unsigned long long nodenr;

    value = endvalue = 0ULL;

    optarg = get_opt_name(option, 128, optarg, ',');
    if (*optarg == ',') {
        optarg++;
    }
    if (!strcmp(option, "node")) {

        if (nb_numa_nodes >= MAX_NODES) {
            fprintf(stderr, "qemu: too many NUMA nodes\n");
            exit(1);
        }

        if (get_param_value(option, 128, "nodeid", optarg) == 0) {
            nodenr = nb_numa_nodes;
        } else {
            nodenr = strtoull(option, NULL, 10);
        }

        if (nodenr >= MAX_NODES) {
            fprintf(stderr, "qemu: invalid NUMA nodeid: %llu\n", nodenr);
            exit(1);
        }

        if (get_param_value(option, 128, "mem", optarg) == 0) {
            node_mem[nodenr] = 0;
        } else {
            int64_t sval;
            sval = strtosz(option, &endptr);
            if (sval < 0 || *endptr) {
                fprintf(stderr, "qemu: invalid numa mem size: %s\n", optarg);
                exit(1);
            }
            node_mem[nodenr] = sval;
        }
        if (get_param_value(option, 128, "cpus", optarg) != 0) {
            value = strtoull(option, &endptr, 10);
            if (*endptr == '-') {
                endvalue = strtoull(endptr+1, &endptr, 10);
            } else {
                endvalue = value;
            }

            if (!(endvalue < MAX_CPUMASK_BITS)) {
                endvalue = MAX_CPUMASK_BITS - 1;
                fprintf(stderr,
                    "A max of %d CPUs are supported in a guest\n",
                     MAX_CPUMASK_BITS);
            }

            bitmap_set(node_cpumask[nodenr], value, endvalue-value+1);
        }
        nb_numa_nodes++;
    } else {
        fprintf(stderr, "Invalid -numa option: %s\n", option);
        exit(1);
    }
}
