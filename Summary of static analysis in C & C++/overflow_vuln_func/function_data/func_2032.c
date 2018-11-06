static inline void vhost_dev_log_resize(struct vhost_dev* dev, uint64_t size)
{
    vhost_log_chunk_t *log;
    uint64_t log_base;
    int r, i;

    log = g_malloc0(size * sizeof *log);
    log_base = (uint64_t)(unsigned long)log;
    r = ioctl(dev->control, VHOST_SET_LOG_BASE, &log_base);
    assert(r >= 0);
    for (i = 0; i < dev->n_mem_sections; ++i) {
        /
        vhost_sync_dirty_bitmap(dev, &dev->mem_sections[i], 0,
                                dev->log_size * VHOST_LOG_CHUNK - 1);
    }
    if (dev->log) {
        g_free(dev->log);
    }
    dev->log = log;
    dev->log_size = size;
}
