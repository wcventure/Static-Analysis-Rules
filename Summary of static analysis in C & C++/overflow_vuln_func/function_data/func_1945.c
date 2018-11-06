FpPort *fp_port_alloc(Rocker *r, char *sw_name,
                      MACAddr *start_mac, unsigned int index,
                      NICPeers *peers)
{
    FpPort *port = g_malloc0(sizeof(FpPort));

    if (!port) {
        return NULL;
    }

    port->r = r;
    port->index = index;
    port->pport = index + 1;

    /

    port->name = g_strdup_printf("%sp%d", sw_name, port->pport);

    memcpy(port->conf.macaddr.a, start_mac, sizeof(port->conf.macaddr.a));
    port->conf.macaddr.a[5] += index;
    port->conf.bootindex = -1;
    port->conf.peers = *peers;

    port->nic = qemu_new_nic(&fp_port_info, &port->conf,
                             sw_name, NULL, port);
    qemu_format_nic_info_str(qemu_get_queue(port->nic),
                             port->conf.macaddr.a);

    fp_port_reset(port);

    return port;
}
