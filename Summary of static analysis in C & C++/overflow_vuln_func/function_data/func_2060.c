static int virtio_gpu_ui_info(void *opaque, uint32_t idx, QemuUIInfo *info)
{
    VirtIOGPU *g = opaque;

    if (idx > g->conf.max_outputs) {
        return -1;
    }

    g->req_state[idx].x = info->xoff;
    g->req_state[idx].y = info->yoff;
    g->req_state[idx].width = info->width;
    g->req_state[idx].height = info->height;

    if (info->width && info->height) {
        g->enabled_output_bitmask |= (1 << idx);
    } else {
        g->enabled_output_bitmask &= ~(1 << idx);
    }

    /
    virtio_gpu_notify_event(g, VIRTIO_GPU_EVENT_DISPLAY);
    return 0;
}
