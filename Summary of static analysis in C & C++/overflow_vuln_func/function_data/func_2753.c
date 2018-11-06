static VncJobQueue *vnc_queue_init(void)
{
    VncJobQueue *queue = g_malloc0(sizeof(VncJobQueue));

    qemu_cond_init(&queue->cond);
    qemu_mutex_init(&queue->mutex);
    QTAILQ_INIT(&queue->jobs);
    return queue;
}
