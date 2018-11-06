struct pxa2xx_pcmcia_s *pxa2xx_pcmcia_init(target_phys_addr_t base)
{
    int iomemtype;
    struct pxa2xx_pcmcia_s *s;

    s = (struct pxa2xx_pcmcia_s *)
            qemu_mallocz(sizeof(struct pxa2xx_pcmcia_s));

    /
    s->io_base = base | 0x00000000;
    iomemtype = cpu_register_io_memory(0, pxa2xx_pcmcia_io_readfn,
                    pxa2xx_pcmcia_io_writefn, s);
    cpu_register_physical_memory(s->io_base, 0x03ffffff, iomemtype);

    /

    /
    s->attr_base = base | 0x08000000;
    iomemtype = cpu_register_io_memory(0, pxa2xx_pcmcia_attr_readfn,
                    pxa2xx_pcmcia_attr_writefn, s);
    cpu_register_physical_memory(s->attr_base, 0x03ffffff, iomemtype);

    /
    s->common_base = base | 0x0c000000;
    iomemtype = cpu_register_io_memory(0, pxa2xx_pcmcia_common_readfn,
                    pxa2xx_pcmcia_common_writefn, s);
    cpu_register_physical_memory(s->common_base, 0x03ffffff, iomemtype);

    if (base == 0x30000000)
        s->slot.slot_string = "PXA PC Card Socket 1";
    else
        s->slot.slot_string = "PXA PC Card Socket 0";
    s->slot.irq = qemu_allocate_irqs(pxa2xx_pcmcia_set_irq, s, 1)[0];
    pcmcia_socket_register(&s->slot);

    return s;
}
