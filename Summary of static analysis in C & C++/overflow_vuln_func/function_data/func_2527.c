static int virtio_net_load(QEMUFile *f, void *opaque, int version_id)
{
    VirtIONet *n = opaque;

    if (version_id < 2 || version_id > VIRTIO_NET_VM_VERSION)
        return -EINVAL;

    virtio_load(&n->vdev, f);

    qemu_get_buffer(f, n->mac, ETH_ALEN);
    n->tx_timer_active = qemu_get_be32(f);
    n->mergeable_rx_bufs = qemu_get_be32(f);

    if (version_id >= 3)
        n->status = qemu_get_be16(f);

    if (version_id >= 4) {
        if (version_id < 8) {
            n->promisc = qemu_get_be32(f);
            n->allmulti = qemu_get_be32(f);
        } else {
            n->promisc = qemu_get_byte(f);
            n->allmulti = qemu_get_byte(f);
        }
    }

    if (version_id >= 5) {
        n->mac_table.in_use = qemu_get_be32(f);
        /
        if (n->mac_table.in_use <= MAC_TABLE_ENTRIES) {
            qemu_get_buffer(f, n->mac_table.macs,
                            n->mac_table.in_use * ETH_ALEN);
        } else if (n->mac_table.in_use) {
            qemu_fseek(f, n->mac_table.in_use * ETH_ALEN, SEEK_CUR);
            n->promisc = 1;
            n->mac_table.in_use = 0;
        }
    }
 
    if (version_id >= 6)
        qemu_get_buffer(f, (uint8_t *)n->vlans, MAX_VLAN >> 3);

    if (version_id >= 7 && qemu_get_be32(f)) {
        fprintf(stderr,
                "virtio-net: saved image requires vnet header support\n");
        exit(1);
    }

    if (n->tx_timer_active) {
        qemu_mod_timer(n->tx_timer,
                       qemu_get_clock(vm_clock) + TX_TIMER_INTERVAL);
    }

    return 0;
}
