VncJob *vnc_job_new(VncState *vs)
{
    VncJob *job = g_malloc0(sizeof(VncJob));

    job->vs = vs;
    vnc_lock_queue(queue);
    QLIST_INIT(&job->rectangles);
    vnc_unlock_queue(queue);
    return job;
}
