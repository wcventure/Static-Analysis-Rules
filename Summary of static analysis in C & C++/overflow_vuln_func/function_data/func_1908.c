void usb_ohci_init_pxa(target_phys_addr_t base, int num_ports, int devfn,
                       qemu_irq irq)
{
    OHCIState *ohci = (OHCIState *)qemu_mallocz(sizeof(OHCIState));

    usb_ohci_init(ohci, num_ports, devfn, irq,
                  OHCI_TYPE_PXA, "OHCI USB");
    ohci->mem_base = base;

    cpu_register_physical_memory(ohci->mem_base, 0xfff, ohci->mem);
}
