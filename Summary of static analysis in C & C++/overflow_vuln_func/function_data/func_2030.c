static int vhost_sync_dirty_bitmap(struct vhost_dev *dev,
                                   MemoryRegionSection *section,
                                   hwaddr start_addr,
                                   hwaddr end_addr)
{
    int i;

    if (!dev->log_enabled || !dev->started) {
        return 0;
    }
    for (i = 0; i < dev->mem->nregions; ++i) {
        struct vhost_memory_region *reg = dev->mem->regions + i;
        vhost_dev_sync_region(dev, section, start_addr, end_addr,
                              reg->guest_phys_addr,
                              range_get_last(reg->guest_phys_addr,
                                             reg->memory_size));
    }
    for (i = 0; i < dev->nvqs; ++i) {
        struct vhost_virtqueue *vq = dev->vqs + i;
        vhost_dev_sync_region(dev, section, start_addr, end_addr, vq->used_phys,
                              range_get_last(vq->used_phys, vq->used_size));
    }
    return 0;
}
