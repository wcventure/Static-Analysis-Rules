int spapr_populate_pci_devices(sPAPRPHBState *phb,
                               uint32_t xics_phandle,
                               void *fdt)
{
    PCIBus *bus = phb->host_state.bus;
    int bus_off, node_off = 0, devid, fn, i, n, devices;
    DeviceState *qdev;
    char nodename[256];
    struct {
        uint32_t hi;
        uint64_t addr;
        uint64_t size;
    } __attribute__((packed)) reg[PCI_NUM_REGIONS + 1],
          assigned_addresses[PCI_NUM_REGIONS];
    uint32_t bus_range[] = { cpu_to_be32(0), cpu_to_be32(0xff) };
    struct {
        uint32_t hi;
        uint64_t child;
        uint64_t parent;
        uint64_t size;
    } __attribute__((packed)) ranges[] = {
        {
            cpu_to_be32(b_ss(1)), cpu_to_be64(0),
            cpu_to_be64(phb->io_win_addr),
            cpu_to_be64(memory_region_size(&phb->iospace)),
        },
        {
            cpu_to_be32(b_ss(2)), cpu_to_be64(SPAPR_PCI_MEM_WIN_BUS_OFFSET),
            cpu_to_be64(phb->mem_win_addr),
            cpu_to_be64(memory_region_size(&phb->memwindow)),
        },
    };
    uint64_t bus_reg[] = { cpu_to_be64(phb->buid), 0 };
    uint32_t interrupt_map_mask[] = {
        cpu_to_be32(b_ddddd(-1)|b_fff(-1)), 0x0, 0x0, 0x0};
    uint32_t interrupt_map[bus->nirq][7];

    /
    sprintf(nodename, "pci@%" PRIx64, phb->buid);
    bus_off = fdt_add_subnode(fdt, 0, nodename);
    if (bus_off < 0) {
        return bus_off;
    }

#define _FDT(exp) \
    do { \
        int ret = (exp);                                           \
        if (ret < 0) {                                             \
            return ret;                                            \
        }                                                          \
    } while (0)

    /
    _FDT(fdt_setprop_string(fdt, bus_off, "device_type", "pci"));
    _FDT(fdt_setprop_string(fdt, bus_off, "compatible", "IBM,Logical_PHB"));
    _FDT(fdt_setprop_cell(fdt, bus_off, "#address-cells", 0x3));
    _FDT(fdt_setprop_cell(fdt, bus_off, "#size-cells", 0x2));
    _FDT(fdt_setprop_cell(fdt, bus_off, "#interrupt-cells", 0x1));
    _FDT(fdt_setprop(fdt, bus_off, "used-by-rtas", NULL, 0));
    _FDT(fdt_setprop(fdt, bus_off, "bus-range", &bus_range, sizeof(bus_range)));
    _FDT(fdt_setprop(fdt, bus_off, "ranges", &ranges, sizeof(ranges)));
    _FDT(fdt_setprop(fdt, bus_off, "reg", &bus_reg, sizeof(bus_reg)));
    _FDT(fdt_setprop(fdt, bus_off, "interrupt-map-mask",
                     &interrupt_map_mask, sizeof(interrupt_map_mask)));

    /
    devices = 0;
    QTAILQ_FOREACH(qdev, &bus->qbus.children, sibling) {
        PCIDevice *dev = DO_UPCAST(PCIDevice, qdev, qdev);
        int irq_index = pci_spapr_map_irq(dev, 0);
        uint32_t *irqmap = interrupt_map[devices];
        uint8_t *config = dev->config;

        devid = dev->devfn >> 3;
        fn = dev->devfn & 7;

        sprintf(nodename, "pci@%u,%u", devid, fn);

        /
        if (devid > bus->nirq)  {
            printf("Unexpected behaviour in spapr_populate_pci_devices,"
                    "wrong devid %u\n", devid);
            exit(-1);
        }
        irqmap[0] = cpu_to_be32(b_ddddd(devid)|b_fff(fn));
        irqmap[1] = 0;
        irqmap[2] = 0;
        irqmap[3] = 0;
        irqmap[4] = cpu_to_be32(xics_phandle);
        irqmap[5] = cpu_to_be32(phb->lsi_table[irq_index].dt_irq);
        irqmap[6] = cpu_to_be32(0x8);

        /
        node_off = fdt_add_subnode(fdt, bus_off, nodename);
        if (node_off < 0) {
            return node_off;
        }

        _FDT(fdt_setprop_cell(fdt, node_off, "vendor-id",
                              pci_get_word(&config[PCI_VENDOR_ID])));
        _FDT(fdt_setprop_cell(fdt, node_off, "device-id",
                              pci_get_word(&config[PCI_DEVICE_ID])));
        _FDT(fdt_setprop_cell(fdt, node_off, "revision-id",
                              pci_get_byte(&config[PCI_REVISION_ID])));
        _FDT(fdt_setprop_cell(fdt, node_off, "class-code",
                              pci_get_long(&config[PCI_CLASS_REVISION]) >> 8));
        _FDT(fdt_setprop_cell(fdt, node_off, "subsystem-id",
                              pci_get_word(&config[PCI_SUBSYSTEM_ID])));
        _FDT(fdt_setprop_cell(fdt, node_off, "subsystem-vendor-id",
                              pci_get_word(&config[PCI_SUBSYSTEM_VENDOR_ID])));

        /
        reg[0].hi = cpu_to_be32(
            b_n(0) |
            b_p(0) |
            b_t(0) |
            b_ss(0/) |
            b_bbbbbbbb(0) |
            b_ddddd(devid) |
            b_fff(fn));
        reg[0].addr = 0;
        reg[0].size = 0;

        n = 0;
        for (i = 0; i < PCI_NUM_REGIONS; ++i) {
            if (0 == dev->io_regions[i].size) {
                continue;
            }

            reg[n+1].hi = cpu_to_be32(
                b_n(0) |
                b_p(0) |
                b_t(0) |
                b_ss(regtype_to_ss(dev->io_regions[i].type)) |
                b_bbbbbbbb(0) |
                b_ddddd(devid) |
                b_fff(fn) |
                b_rrrrrrrr(bars[i]));
            reg[n+1].addr = 0;
            reg[n+1].size = cpu_to_be64(dev->io_regions[i].size);

            assigned_addresses[n].hi = cpu_to_be32(
                b_n(1) |
                b_p(0) |
                b_t(0) |
                b_ss(regtype_to_ss(dev->io_regions[i].type)) |
                b_bbbbbbbb(0) |
                b_ddddd(devid) |
                b_fff(fn) |
                b_rrrrrrrr(bars[i]));

            /
            assigned_addresses[n].addr = cpu_to_be64(dev->io_regions[i].addr);
            assigned_addresses[n].size = reg[n+1].size;

            ++n;
        }
        _FDT(fdt_setprop(fdt, node_off, "reg", reg, sizeof(reg[0])*(n+1)));
        _FDT(fdt_setprop(fdt, node_off, "assigned-addresses",
                         assigned_addresses,
                         sizeof(assigned_addresses[0])*(n)));
        _FDT(fdt_setprop_cell(fdt, node_off, "interrupts",
                              pci_get_byte(&config[PCI_INTERRUPT_PIN])));

        ++devices;
    }

    /
    _FDT(fdt_setprop(fdt, bus_off, "interrupt-map", &interrupt_map,
                     devices * sizeof(interrupt_map[0])));

    return 0;
}
