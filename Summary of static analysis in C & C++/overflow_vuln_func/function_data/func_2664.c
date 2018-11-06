void pci_ne2000_init(PCIBus *bus, NICInfo *nd)
{
    PCINE2000State *d;
    NE2000State *s;
    uint8_t *pci_conf;
    
    d = (PCINE2000State *)pci_register_device(bus,
                                              "NE2000", sizeof(PCINE2000State),
                                              -1, 
                                              NULL, NULL);
    pci_conf = d->dev.config;
    pci_conf[0x00] = 0xec; // Realtek 8029
    pci_conf[0x01] = 0x10;
    pci_conf[0x02] = 0x29;
    pci_conf[0x03] = 0x80;
    pci_conf[0x0a] = 0x00; // ethernet network controller 
    pci_conf[0x0b] = 0x02;
    pci_conf[0x0e] = 0x00; // header_type
    pci_conf[0x3d] = 1; // interrupt pin 0
    
    pci_register_io_region(&d->dev, 0, 0x100, 
                           PCI_ADDRESS_SPACE_IO, ne2000_map);
    s = &d->ne2000;
    s->irq = 16; // PCI interrupt
    s->pci_dev = (PCIDevice *)d;
    memcpy(s->macaddr, nd->macaddr, 6);
    ne2000_reset(s);
    s->vc = qemu_new_vlan_client(nd->vlan, ne2000_receive, s);

    snprintf(s->vc->info_str, sizeof(s->vc->info_str),
             "ne2000 pci macaddr=%02x:%02x:%02x:%02x:%02x:%02x",
             s->macaddr[0],
             s->macaddr[1],
             s->macaddr[2],
             s->macaddr[3],
             s->macaddr[4],
             s->macaddr[5]);
             
    /
    register_savevm("ne2000", 0, 2, ne2000_save, ne2000_load, s);
    register_savevm("ne2000_pci", 0, 1, generic_pci_save, generic_pci_load, 
                    &d->dev);
}
