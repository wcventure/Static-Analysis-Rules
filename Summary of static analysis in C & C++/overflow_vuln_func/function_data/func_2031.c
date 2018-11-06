static void vhost_log_sync(MemoryListener *listener,
                          MemoryRegionSection *section)
{
    struct vhost_dev *dev = container_of(listener, struct vhost_dev,
                                         memory_listener);
    hwaddr start_addr = section->offset_within_address_space;
    hwaddr end_addr = start_addr + section->size;

    vhost_sync_dirty_bitmap(dev, section, start_addr, end_addr);
}
