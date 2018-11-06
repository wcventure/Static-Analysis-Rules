void vhost_dev_stop(struct vhost_dev *hdev, VirtIODevice *vdev)
{
    int i;

    for (i = 0; i < hdev->nvqs; ++i) {
        vhost_virtqueue_stop(hdev,
                             vdev,
                             hdev->vqs + i,
                             hdev->vq_index + i);
    }
    for (i = 0; i < hdev->n_mem_sections; ++i) {
        vhost_sync_dirty_bitmap(hdev, &hdev->mem_sections[i],
                                0, (hwaddr)~0x0ull);
    }

    hdev->started = false;
    g_free(hdev->log);
    hdev->log = NULL;
    hdev->log_size = 0;
}
