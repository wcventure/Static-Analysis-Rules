void isa_ne2000_init(int base, int irq, NICInfo *nd)
{
    NE2000State *s;
    
    s = qemu_mallocz(sizeof(NE2000State));
    if (!s)
        return;
    
    register_ioport_write(base, 16, 1, ne2000_ioport_write, s);
    register_ioport_read(base, 16, 1, ne2000_ioport_read, s);

    register_ioport_write(base + 0x10, 1, 1, ne2000_asic_ioport_write, s);
    register_ioport_read(base + 0x10, 1, 1, ne2000_asic_ioport_read, s);
    register_ioport_write(base + 0x10, 2, 2, ne2000_asic_ioport_write, s);
    register_ioport_read(base + 0x10, 2, 2, ne2000_asic_ioport_read, s);

    register_ioport_write(base + 0x1f, 1, 1, ne2000_reset_ioport_write, s);
    register_ioport_read(base + 0x1f, 1, 1, ne2000_reset_ioport_read, s);
    s->irq = irq;
    memcpy(s->macaddr, nd->macaddr, 6);

    ne2000_reset(s);

    s->vc = qemu_new_vlan_client(nd->vlan, ne2000_receive, s);

    snprintf(s->vc->info_str, sizeof(s->vc->info_str),
             "ne2000 macaddr=%02x:%02x:%02x:%02x:%02x:%02x",
             s->macaddr[0],
             s->macaddr[1],
             s->macaddr[2],
             s->macaddr[3],
             s->macaddr[4],
             s->macaddr[5]);
             
    register_savevm("ne2000", 0, 2, ne2000_save, ne2000_load, s);
}
