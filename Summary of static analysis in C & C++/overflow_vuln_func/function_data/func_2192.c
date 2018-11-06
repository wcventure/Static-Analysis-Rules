void virtqueue_get_avail_bytes(VirtQueue *vq, unsigned int *in_bytes,
                               unsigned int *out_bytes,
                               unsigned max_in_bytes, unsigned max_out_bytes)
{
    unsigned int idx;
    unsigned int total_bufs, in_total, out_total;
    int rc;

    idx = vq->last_avail_idx;

    total_bufs = in_total = out_total = 0;
    while ((rc = virtqueue_num_heads(vq, idx)) > 0) {
        VirtIODevice *vdev = vq->vdev;
        unsigned int max, num_bufs, indirect = 0;
        VRingDesc desc;
        hwaddr desc_pa;
        unsigned int i;

        max = vq->vring.num;
        num_bufs = total_bufs;
        i = virtqueue_get_head(vq, idx++);
        desc_pa = vq->vring.desc;
        vring_desc_read(vdev, &desc, desc_pa, i);

        if (desc.flags & VRING_DESC_F_INDIRECT) {
            if (desc.len % sizeof(VRingDesc)) {
                virtio_error(vdev, "Invalid size for indirect buffer table");
                goto err;
            }

            /
            if (num_bufs >= max) {
                virtio_error(vdev, "Looped descriptor");
                goto err;
            }

            /
            indirect = 1;
            max = desc.len / sizeof(VRingDesc);
            desc_pa = desc.addr;
            num_bufs = i = 0;
            vring_desc_read(vdev, &desc, desc_pa, i);
        }

        do {
            /
            if (++num_bufs > max) {
                virtio_error(vdev, "Looped descriptor");
                goto err;
            }

            if (desc.flags & VRING_DESC_F_WRITE) {
                in_total += desc.len;
            } else {
                out_total += desc.len;
            }
            if (in_total >= max_in_bytes && out_total >= max_out_bytes) {
                goto done;
            }

            rc = virtqueue_read_next_desc(vdev, &desc, desc_pa, max, &i);
        } while (rc == VIRTQUEUE_READ_DESC_MORE);

        if (rc == VIRTQUEUE_READ_DESC_ERROR) {
            goto err;
        }

        if (!indirect)
            total_bufs = num_bufs;
        else
            total_bufs++;
    }

    if (rc < 0) {
        goto err;
    }

done:
    if (in_bytes) {
        *in_bytes = in_total;
    }
    if (out_bytes) {
        *out_bytes = out_total;
    }
    return;

err:
    in_total = out_total = 0;
    goto done;
}
