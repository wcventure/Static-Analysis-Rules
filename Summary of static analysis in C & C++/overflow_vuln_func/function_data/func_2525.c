static int virtio_net_handle_mac(VirtIONet *n, uint8_t cmd,
                                 VirtQueueElement *elem)
{
    struct virtio_net_ctrl_mac mac_data;

    if (cmd != VIRTIO_NET_CTRL_MAC_TABLE_SET || elem->out_num != 3 ||
        elem->out_sg[1].iov_len < sizeof(mac_data) ||
        elem->out_sg[2].iov_len < sizeof(mac_data))
        return VIRTIO_NET_ERR;

    n->mac_table.in_use = 0;
    memset(n->mac_table.macs, 0, MAC_TABLE_ENTRIES * ETH_ALEN);

    mac_data.entries = ldl_le_p(elem->out_sg[1].iov_base);

    if (sizeof(mac_data.entries) +
        (mac_data.entries * ETH_ALEN) > elem->out_sg[1].iov_len)
        return VIRTIO_NET_ERR;

    if (mac_data.entries <= MAC_TABLE_ENTRIES) {
        memcpy(n->mac_table.macs, elem->out_sg[1].iov_base + sizeof(mac_data),
               mac_data.entries * ETH_ALEN);
        n->mac_table.in_use += mac_data.entries;
    } else {
        n->promisc = 1;
        return VIRTIO_NET_OK;
    }

    mac_data.entries = ldl_le_p(elem->out_sg[2].iov_base);

    if (sizeof(mac_data.entries) +
        (mac_data.entries * ETH_ALEN) > elem->out_sg[2].iov_len)
        return VIRTIO_NET_ERR;

    if (mac_data.entries) {
        if (n->mac_table.in_use + mac_data.entries <= MAC_TABLE_ENTRIES) {
            memcpy(n->mac_table.macs + (n->mac_table.in_use * ETH_ALEN),
                   elem->out_sg[2].iov_base + sizeof(mac_data),
                   mac_data.entries * ETH_ALEN);
            n->mac_table.in_use += mac_data.entries;
        } else
            n->allmulti = 1;
    }

    return VIRTIO_NET_OK;
}
