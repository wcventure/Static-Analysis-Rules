int vnc_job_add_rect(VncJob *job, int x, int y, int w, int h)
{
    VncRectEntry *entry = g_malloc0(sizeof(VncRectEntry));

    entry->rect.x = x;
    entry->rect.y = y;
    entry->rect.w = w;
    entry->rect.h = h;

    vnc_lock_queue(queue);
    QLIST_INSERT_HEAD(&job->rectangles, entry, next);
    vnc_unlock_queue(queue);
    return 1;
}
