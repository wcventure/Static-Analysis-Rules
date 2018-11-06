DescRing *desc_ring_alloc(Rocker *r, int index)
{
    DescRing *ring;

    ring = g_malloc0(sizeof(DescRing));
    if (!ring) {
        return NULL;
    }

    ring->r = r;
    ring->index = index;

    return ring;
}
