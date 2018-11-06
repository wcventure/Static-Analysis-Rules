bool desc_ring_set_size(DescRing *ring, uint32_t size)
{
    int i;

    if (size < 2 || size > 0x10000 || (size & (size - 1))) {
        DPRINTF("ERROR: ring[%d] size (%d) not a power of 2 "
                "or in range [2, 64K]\n", ring->index, size);
        return false;
    }

    for (i = 0; i < ring->size; i++) {
        g_free(ring->info[i].buf);
    }

    ring->size = size;
    ring->head = ring->tail = 0;

    ring->info = g_realloc(ring->info, size * sizeof(DescInfo));
    if (!ring->info) {
        return false;
    }

    memset(ring->info, 0, size * sizeof(DescInfo));

    for (i = 0; i < size; i++) {
        ring->info[i].ring = ring;
    }

    return true;
}
