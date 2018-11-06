void *virtqueue_pop(VirtQueue *vq, size_t sz)
{
    unsigned int i, head, max;
    hwaddr desc_pa = vq->vring.desc;
    VirtIODevice *vdev = vq->vdev;
    VirtQueueElement *elem;
    unsigned out_num, in_num;
    hwaddr addr[VIRTQUEUE_MAX_SIZE];
    struct iovec iov[VIRTQUEUE_MAX_SIZE];
    VRingDesc desc;
    int rc;

    if (unlikely(vdev->broken)) {
        return NULL;
    }
    if (virtio_queue_empty(vq)) {
        return NULL;
    }
    /
    smp_rmb();

    /
    out_num = in_num = 0;

    max = vq->vring.num;

    if (vq->inuse >= vq->vring.num) {
        virtio_error(vdev, "Virtqueue size exceeded");
        return NULL;
    }

    i = head = virtqueue_get_head(vq, vq->last_avail_idx++);
    if (virtio_vdev_has_feature(vdev, VIRTIO_RING_F_EVENT_IDX)) {
        vring_set_avail_event(vq, vq->last_avail_idx);
    }

    vring_desc_read(vdev, &desc, desc_pa, i);
    if (desc.flags & VRING_DESC_F_INDIRECT) {
        if (desc.len % sizeof(VRingDesc)) {
            virtio_error(vdev, "Invalid size for indirect buffer table");
            return NULL;
        }

        /
        max = desc.len / sizeof(VRingDesc);
        desc_pa = desc.addr;
        i = 0;
        vring_desc_read(vdev, &desc, desc_pa, i);
    }

    /
    do {
        bool map_ok;

        if (desc.flags & VRING_DESC_F_WRITE) {
            map_ok = virtqueue_map_desc(vdev, &in_num, addr + out_num,
                                        iov + out_num,
                                        VIRTQUEUE_MAX_SIZE - out_num, true,
                                        desc.addr, desc.len);
        } else {
            if (in_num) {
                virtio_error(vdev, "Incorrect order for descriptors");
                goto err_undo_map;
            }
            map_ok = virtqueue_map_desc(vdev, &out_num, addr, iov,
                                        VIRTQUEUE_MAX_SIZE, false,
                                        desc.addr, desc.len);
        }
        if (!map_ok) {
            goto err_undo_map;
        }

        /
        if ((in_num + out_num) > max) {
            virtio_error(vdev, "Looped descriptor");
            goto err_undo_map;
        }

        rc = virtqueue_read_next_desc(vdev, &desc, desc_pa, max, &i);
    } while (rc == VIRTQUEUE_READ_DESC_MORE);

    if (rc == VIRTQUEUE_READ_DESC_ERROR) {
        goto err_undo_map;
    }

    /
    elem = virtqueue_alloc_element(sz, out_num, in_num);
    elem->index = head;
    for (i = 0; i < out_num; i++) {
        elem->out_addr[i] = addr[i];
        elem->out_sg[i] = iov[i];
    }
    for (i = 0; i < in_num; i++) {
        elem->in_addr[i] = addr[out_num + i];
        elem->in_sg[i] = iov[out_num + i];
    }

    vq->inuse++;

    trace_virtqueue_pop(vq, elem, elem->in_num, elem->out_num);
    return elem;

err_undo_map:
    virtqueue_undo_map_desc(out_num, in_num, iov);
    return NULL;
}
