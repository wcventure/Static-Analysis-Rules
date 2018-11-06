static void pty_chr_state(CharDriverState *chr, int connected)
{
    PtyCharDriver *s = chr->opaque;

    if (!connected) {
        if (s->fd_tag) {
            io_remove_watch_poll(s->fd_tag);
            s->fd_tag = 0;
        }
        s->connected = 0;
        /
        pty_chr_rearm_timer(chr, 1000);
    } else {
        if (s->timer_tag) {
            g_source_remove(s->timer_tag);
            s->timer_tag = 0;
        }
        if (!s->connected) {
            qemu_chr_be_generic_open(chr);
            s->connected = 1;
            s->fd_tag = io_add_watch_poll(s->fd, pty_chr_read_poll, pty_chr_read, chr);
        }
    }
}
