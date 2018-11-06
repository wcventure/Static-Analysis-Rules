static CharDriverState *qemu_chr_open_pty(const char *id,
                                          ChardevReturn *ret)
{
    CharDriverState *chr;
    PtyCharDriver *s;
    int master_fd, slave_fd;
    char pty_name[PATH_MAX];

    master_fd = qemu_openpty_raw(&slave_fd, pty_name);
    if (master_fd < 0) {
        return NULL;
    }

    close(slave_fd);
    qemu_set_nonblock(master_fd);

    chr = qemu_chr_alloc();

    chr->filename = g_strdup_printf("pty:%s", pty_name);
    ret->pty = g_strdup(pty_name);
    ret->has_pty = true;

    fprintf(stderr, "char device redirected to %s (label %s)\n",
            pty_name, id);

    s = g_malloc0(sizeof(PtyCharDriver));
    chr->opaque = s;
    chr->chr_write = pty_chr_write;
    chr->chr_update_read_handler = pty_chr_update_read_handler;
    chr->chr_close = pty_chr_close;
    chr->chr_add_watch = pty_chr_add_watch;
    chr->explicit_be_open = true;

    s->fd = io_channel_from_fd(master_fd);
    s->timer_tag = 0;

    return chr;
}
