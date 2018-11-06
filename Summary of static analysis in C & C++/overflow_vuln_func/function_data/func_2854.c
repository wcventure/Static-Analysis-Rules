static uint64_t virtio_pci_common_read(void *opaque, hwaddr addr,
                                       unsigned size)
{
    VirtIOPCIProxy *proxy = opaque;
    VirtIODevice *vdev = virtio_bus_get_device(&proxy->bus);
    uint32_t val = 0;
    int i;

    switch (addr) {
    case VIRTIO_PCI_COMMON_DFSELECT:
        val = proxy->dfselect;
        break;
    case VIRTIO_PCI_COMMON_DF:
        if (proxy->dfselect <= 1) {
            val = vdev->host_features >> (32 * proxy->dfselect);
        }
        break;
    case VIRTIO_PCI_COMMON_GFSELECT:
        val = proxy->gfselect;
        break;
    case VIRTIO_PCI_COMMON_GF:
        if (proxy->gfselect <= ARRAY_SIZE(proxy->guest_features)) {
            val = proxy->guest_features[proxy->gfselect];
        }
        break;
    case VIRTIO_PCI_COMMON_MSIX:
        val = vdev->config_vector;
        break;
    case VIRTIO_PCI_COMMON_NUMQ:
        for (i = 0; i < VIRTIO_QUEUE_MAX; ++i) {
            if (virtio_queue_get_num(vdev, i)) {
                val = i + 1;
            }
        }
        break;
    case VIRTIO_PCI_COMMON_STATUS:
        val = vdev->status;
        break;
    case VIRTIO_PCI_COMMON_CFGGENERATION:
        val = vdev->generation;
        break;
    case VIRTIO_PCI_COMMON_Q_SELECT:
        val = vdev->queue_sel;
        break;
    case VIRTIO_PCI_COMMON_Q_SIZE:
        val = virtio_queue_get_num(vdev, vdev->queue_sel);
        break;
    case VIRTIO_PCI_COMMON_Q_MSIX:
        val = virtio_queue_vector(vdev, vdev->queue_sel);
        break;
    case VIRTIO_PCI_COMMON_Q_ENABLE:
        val = proxy->vqs[vdev->queue_sel].enabled;
        break;
    case VIRTIO_PCI_COMMON_Q_NOFF:
        /
        val = vdev->queue_sel;
        break;
    case VIRTIO_PCI_COMMON_Q_DESCLO:
        val = proxy->vqs[vdev->queue_sel].desc[0];
        break;
    case VIRTIO_PCI_COMMON_Q_DESCHI:
        val = proxy->vqs[vdev->queue_sel].desc[1];
        break;
    case VIRTIO_PCI_COMMON_Q_AVAILLO:
        val = proxy->vqs[vdev->queue_sel].avail[0];
        break;
    case VIRTIO_PCI_COMMON_Q_AVAILHI:
        val = proxy->vqs[vdev->queue_sel].avail[1];
        break;
    case VIRTIO_PCI_COMMON_Q_USEDLO:
        val = proxy->vqs[vdev->queue_sel].used[0];
        break;
    case VIRTIO_PCI_COMMON_Q_USEDHI:
        val = proxy->vqs[vdev->queue_sel].used[1];
        break;
    default:
        val = 0;
    }

    return val;
}
