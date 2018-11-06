static void
iscsi_set_events(IscsiLun *iscsilun)
{
    struct iscsi_context *iscsi = iscsilun->iscsi;
    int ev = iscsi_which_events(iscsi);

    if (ev != iscsilun->events) {
        aio_set_fd_handler(iscsilun->aio_context,
                           iscsi_get_fd(iscsi),
                           (ev & POLLIN) ? iscsi_process_read : NULL,
                           (ev & POLLOUT) ? iscsi_process_write : NULL,
                           iscsilun);
        iscsilun->events = ev;
    }

    /
    if (!ev) {
        timer_mod(iscsilun->event_timer,
                  qemu_clock_get_ms(QEMU_CLOCK_REALTIME) + EVENT_INTERVAL);
    }
}
