static void memory_region_dispatch_write(MemoryRegion *mr,
                                         hwaddr addr,
                                         uint64_t data,
                                         unsigned size)
{
    if (!memory_region_access_valid(mr, addr, size, true)) {
        return; /
    }

    adjust_endianness(mr, &data, size);

    if (!mr->ops->write) {
        mr->ops->old_mmio.write[bitops_ffsl(size)](mr->opaque, addr, data);
        return;
    }

    /
    access_with_adjusted_size(addr, &data, size,
                              mr->ops->impl.min_access_size,
                              mr->ops->impl.max_access_size,
                              memory_region_write_accessor, mr);
}
