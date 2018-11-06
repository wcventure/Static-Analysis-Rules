static void virtio_net_add_queue(VirtIONet *n, int index)
{
    VirtIODevice *vdev = VIRTIO_DEVICE(n);

    n->vqs[index].rx_vq = virtio_add_queue(vdev, 256, virtio_net_handle_rx);
    if (n->net_conf.tx && !strcmp(n->net_conf.tx, "timer")) {
        n->vqs[index].tx_vq =
            virtio_add_queue(vdev, 256, virtio_net_handle_tx_timer);
        n->vqs[index].tx_timer = timer_new_ns(QEMU_CLOCK_VIRTUAL,
                                              virtio_net_tx_timer,
                                              &n->vqs[index]);
    } else {
        n->vqs[index].tx_vq =
            virtio_add_queue(vdev, 256, virtio_net_handle_tx_bh);
        n->vqs[index].tx_bh = qemu_bh_new(virtio_net_tx_bh, &n->vqs[index]);
    }

    n->vqs[index].tx_waiting = 0;
    n->vqs[index].n = n;
}
