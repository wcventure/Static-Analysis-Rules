static void reclaim_ramblock(RAMBlock *block)
{
    if (block->flags & RAM_PREALLOC) {
        ;
    } else if (xen_enabled()) {
        xen_invalidate_map_cache_entry(block->host);
#ifndef _WIN32
    } else if (block->fd >= 0) {
        munmap(block->host, block->max_length);
        close(block->fd);
#endif
    } else {
        qemu_anon_ram_free(block->host, block->max_length);
    }
    g_free(block);
}
