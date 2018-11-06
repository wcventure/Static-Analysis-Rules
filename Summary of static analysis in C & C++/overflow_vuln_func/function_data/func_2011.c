static void spapr_phb_placement(sPAPRMachineState *spapr, uint32_t index,
                                uint64_t *buid, hwaddr *pio,
                                hwaddr *mmio32, hwaddr *mmio64,
                                unsigned n_dma, uint32_t *liobns, Error **errp)
{
    const uint64_t base_buid = 0x800000020000000ULL;
    const hwaddr phb_spacing = 0x1000000000ULL; /
    const hwaddr mmio_offset = 0xa0000000; /
    const hwaddr pio_offset = 0x80000000; /
    const uint32_t max_index = 255;
    const hwaddr phb0_alignment = 0x10000000000ULL; /

    uint64_t ram_top = MACHINE(spapr)->ram_size;
    hwaddr phb0_base, phb_base;
    int i;

    /
    if (MACHINE(spapr)->maxram_size > ram_top) {
        /
        ram_top = spapr->hotplug_memory.base +
            memory_region_size(&spapr->hotplug_memory.mr);
    }

    phb0_base = QEMU_ALIGN_UP(ram_top, phb0_alignment);

    if (index > max_index) {
        error_setg(errp, "\"index\" for PAPR PHB is too large (max %u)",
                   max_index);
        return;
    }

    *buid = base_buid + index;
    for (i = 0; i < n_dma; ++i) {
        liobns[i] = SPAPR_PCI_LIOBN(index, i);
    }

    phb_base = phb0_base + index * phb_spacing;
    *pio = phb_base + pio_offset;
    *mmio32 = phb_base + mmio_offset;
    /
}
