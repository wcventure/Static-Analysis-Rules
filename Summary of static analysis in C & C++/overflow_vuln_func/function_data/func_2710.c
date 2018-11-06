void virtio_scsi_common_realize(DeviceState *dev, Error **errp,
                                HandleOutput ctrl, HandleOutput evt,
                                HandleOutput cmd)
{
    VirtIODevice *vdev = VIRTIO_DEVICE(dev);
    VirtIOSCSICommon *s = VIRTIO_SCSI_COMMON(dev);
    int i;

    virtio_init(vdev, "virtio-scsi", VIRTIO_ID_SCSI,
                sizeof(VirtIOSCSIConfig));

    if (s->conf.num_queues == 0 ||
            s->conf.num_queues > VIRTIO_PCI_QUEUE_MAX - 2) {
        error_setg(errp, "Invalid number of queues (= %" PRIu32 "), "
                         "must be a positive integer less than %d.",
                   s->conf.num_queues, VIRTIO_PCI_QUEUE_MAX - 2);
        virtio_cleanup(vdev);
        return;
    }
    s->cmd_vqs = g_malloc0(s->conf.num_queues * sizeof(VirtQueue *));
    s->sense_size = VIRTIO_SCSI_SENSE_SIZE;
    s->cdb_size = VIRTIO_SCSI_CDB_SIZE;

    s->ctrl_vq = virtio_add_queue(vdev, VIRTIO_SCSI_VQ_SIZE,
                                  ctrl);
    s->event_vq = virtio_add_queue(vdev, VIRTIO_SCSI_VQ_SIZE,
                                   evt);
    for (i = 0; i < s->conf.num_queues; i++) {
        s->cmd_vqs[i] = virtio_add_queue(vdev, VIRTIO_SCSI_VQ_SIZE,
                                         cmd);
    }

    if (s->conf.iothread) {
        virtio_scsi_set_iothread(VIRTIO_SCSI(s), s->conf.iothread);
    }
}
