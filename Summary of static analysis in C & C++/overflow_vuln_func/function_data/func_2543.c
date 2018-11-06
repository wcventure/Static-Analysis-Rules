void memory_region_notify_one(IOMMUNotifier *notifier,
                              IOMMUTLBEntry *entry)
{
    IOMMUNotifierFlag request_flags;

    /
    if (notifier->start > entry->iova + entry->addr_mask + 1 ||
        notifier->end < entry->iova) {
        return;
    }

    if (entry->perm & IOMMU_RW) {
        request_flags = IOMMU_NOTIFIER_MAP;
    } else {
        request_flags = IOMMU_NOTIFIER_UNMAP;
    }

    if (notifier->notifier_flags & request_flags) {
        notifier->notify(notifier, entry);
    }
}
