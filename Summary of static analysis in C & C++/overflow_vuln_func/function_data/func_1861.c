NetQueue *qemu_new_net_queue(void *opaque)
{
    NetQueue *queue;

    queue = g_malloc0(sizeof(NetQueue));

    queue->opaque = opaque;
    queue->nq_maxlen = 10000;
    queue->nq_count = 0;

    QTAILQ_INIT(&queue->packets);

    queue->delivering = 0;

    return queue;
}
