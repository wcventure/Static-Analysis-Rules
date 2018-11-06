static void virtio_pci_common_write(void *opaque, hwaddr addr,
                                    uint64_t val, unsigned size)
{
    VirtIOPCIProxy *proxy = opaque;
    VirtIODevice *vdev = virtio_bus_get_device(&proxy->bus);

    switch (addr) {
    case VIRTIO_PCI_COMMON_DFSELECT:
        proxy->dfselect = val;
        break;
    case VIRTIO_PCI_COMMON_GFSELECT:
        proxy->gfselect = val;
        break;
    case VIRTIO_PCI_COMMON_GF:
        if (proxy->gfselect <= ARRAY_SIZE(proxy->guest_features)) {
            proxy->guest_features[proxy->gfselect] = val;
            virtio_set_features(vdev,
                                (((uint64_t)proxy->guest_features[1]) << 32) |
                                proxy->guest_features[0]);
        }
        break;
    case VIRTIO_PCI_COMMON_MSIX:
        msix_vector_unuse(&proxy->pci_dev, vdev->config_vector);
        /
        if (msix_vector_use(&proxy->pci_dev, val) < 0) {
            val = VIRTIO_NO_VECTOR;
        }
        vdev->config_vector = val;
        break;
    case VIRTIO_PCI_COMMON_STATUS:
        if (!(val & VIRTIO_CONFIG_S_DRIVER_OK)) {
            virtio_pci_stop_ioeventfd(proxy);
        }

        virtio_set_status(vdev, val & 0xFF);

        if (val & VIRTIO_CONFIG_S_DRIVER_OK) {
            virtio_pci_start_ioeventfd(proxy);
        }

        if (vdev->status == 0) {
            virtio_reset(vdev);
            msix_unuse_all_vectors(&proxy->pci_dev);
        }

        break;
    case VIRTIO_PCI_COMMON_Q_SELECT:
        if (val < VIRTIO_QUEUE_MAX) {
            vdev->queue_sel = val;
        }
        break;
    case VIRTIO_PCI_COMMON_Q_SIZE:
        proxy->vqs[vdev->queue_sel].num = val;
        break;
    case VIRTIO_PCI_COMMON_Q_MSIX:
        msix_vector_unuse(&proxy->pci_dev,
                          virtio_queue_vector(vdev, vdev->queue_sel));
        /
        if (msix_vector_use(&proxy->pci_dev, val) < 0) {
            val = VIRTIO_NO_VECTOR;
        }
        virtio_queue_set_vector(vdev, vdev->queue_sel, val);
        break;
    case VIRTIO_PCI_COMMON_Q_ENABLE:
        /
        virtio_queue_set_num(vdev, vdev->queue_sel,
                             proxy->vqs[vdev->queue_sel].num);
        virtio_queue_set_rings(vdev, vdev->queue_sel,
                       ((uint64_t)proxy->vqs[vdev->queue_sel].desc[1]) << 32 |
                       proxy->vqs[vdev->queue_sel].desc[0],
                       ((uint64_t)proxy->vqs[vdev->queue_sel].avail[1]) << 32 |
                       proxy->vqs[vdev->queue_sel].avail[0],
                       ((uint64_t)proxy->vqs[vdev->queue_sel].used[1]) << 32 |
                       proxy->vqs[vdev->queue_sel].used[0]);
        break;
    case VIRTIO_PCI_COMMON_Q_DESCLO:
        proxy->vqs[vdev->queue_sel].desc[0] = val;
        break;
    case VIRTIO_PCI_COMMON_Q_DESCHI:
        proxy->vqs[vdev->queue_sel].desc[1] = val;
        break;
    case VIRTIO_PCI_COMMON_Q_AVAILLO:
        proxy->vqs[vdev->queue_sel].avail[0] = val;
        break;
    case VIRTIO_PCI_COMMON_Q_AVAILHI:
        proxy->vqs[vdev->queue_sel].avail[1] = val;
        break;
    case VIRTIO_PCI_COMMON_Q_USEDLO:
        proxy->vqs[vdev->queue_sel].used[0] = val;
        break;
    case VIRTIO_PCI_COMMON_Q_USEDHI:
        proxy->vqs[vdev->queue_sel].used[1] = val;
        break;
    default:
        break;
    }
}
