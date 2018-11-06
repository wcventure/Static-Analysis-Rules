static void virtio_balloon_set_config(VirtIODevice *vdev,
                                      const uint8_t *config_data)
{
    VirtIOBalloon *dev = VIRTIO_BALLOON(vdev);
    struct virtio_balloon_config config;
    uint32_t oldactual = dev->actual;
    memcpy(&config, config_data, 8);
    dev->actual = le32_to_cpu(config.actual);
    if (dev->actual != oldactual) {
        qemu_balloon_changed(ram_size -
                             (dev->actual << VIRTIO_BALLOON_PFN_SHIFT));
    }
}
