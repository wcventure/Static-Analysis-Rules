static void packet_id_queue_add(struct PacketIdQueue *q, uint64_t id)
{
    USBRedirDevice *dev = q->dev;
    struct PacketIdQueueEntry *e;

    DPRINTF("adding packet id %"PRIu64" to %s queue\n", id, q->name);

    e = g_malloc0(sizeof(struct PacketIdQueueEntry));
    e->id = id;
    QTAILQ_INSERT_TAIL(&q->head, e, next);
    q->size++;
}
