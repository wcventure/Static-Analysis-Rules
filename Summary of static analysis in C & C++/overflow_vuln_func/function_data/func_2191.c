static unsigned int virtqueue_get_head(VirtQueue *vq, unsigned int idx)
{
    unsigned int head;

    /
    head = vring_avail_ring(vq, idx % vq->vring.num);

    /
    if (head >= vq->vring.num) {
        error_report("Guest says index %u is available", head);
        exit(1);
    }

    return head;
}
