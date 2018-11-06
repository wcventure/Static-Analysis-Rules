void lance_init(NICInfo *nd, int irq, uint32_t leaddr, uint32_t ledaddr)
{
    LANCEState *s;
    int lance_io_memory, ledma_io_memory;

    s = qemu_mallocz(sizeof(LANCEState));
    if (!s)
        return;

    s->irq = irq;

    lance_io_memory = cpu_register_io_memory(0, lance_mem_read, lance_mem_write, s);
    cpu_register_physical_memory(leaddr, 4, lance_io_memory);

    ledma_io_memory = cpu_register_io_memory(0, ledma_mem_read, ledma_mem_write, s);
    cpu_register_physical_memory(ledaddr, 16, ledma_io_memory);

    memcpy(s->macaddr, nd->macaddr, 6);

    lance_reset(s);

    s->vc = qemu_new_vlan_client(nd->vlan, lance_receive, s);

    snprintf(s->vc->info_str, sizeof(s->vc->info_str),
             "lance macaddr=%02x:%02x:%02x:%02x:%02x:%02x",
             s->macaddr[0],
             s->macaddr[1],
             s->macaddr[2],
             s->macaddr[3],
             s->macaddr[4],
             s->macaddr[5]);

    register_savevm("lance", leaddr, 1, lance_save, lance_load, s);
    qemu_register_reset(lance_reset, s);
}
