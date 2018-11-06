static QObject *get_stats_qobject(VirtIOBalloon *dev)
{
    QDict *dict = qdict_new();
    uint32_t actual = ram_size - (dev->actual << VIRTIO_BALLOON_PFN_SHIFT);

    stat_put(dict, "actual", actual);
    stat_put(dict, "mem_swapped_in", dev->stats[VIRTIO_BALLOON_S_SWAP_IN]);
    stat_put(dict, "mem_swapped_out", dev->stats[VIRTIO_BALLOON_S_SWAP_OUT]);
    stat_put(dict, "major_page_faults", dev->stats[VIRTIO_BALLOON_S_MAJFLT]);
    stat_put(dict, "minor_page_faults", dev->stats[VIRTIO_BALLOON_S_MINFLT]);
    stat_put(dict, "free_mem", dev->stats[VIRTIO_BALLOON_S_MEMFREE]);
    stat_put(dict, "total_mem", dev->stats[VIRTIO_BALLOON_S_MEMTOT]);

    return QOBJECT(dict);
}
