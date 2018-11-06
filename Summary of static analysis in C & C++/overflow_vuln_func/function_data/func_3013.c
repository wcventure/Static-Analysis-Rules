static CharDriverState *qemu_chr_open_fd(int fd_in, int fd_out)
{
    CharDriverState *chr;
    FDCharDriver *s;

    chr = qemu_chr_alloc();
    s = g_malloc0(sizeof(FDCharDriver));
    s->fd_in = io_channel_from_fd(fd_in);
    s->fd_out = io_channel_from_fd(fd_out);
    qemu_set_nonblock(fd_out);
    s->chr = chr;
    chr->opaque = s;
    chr->chr_add_watch = fd_chr_add_watch;
    chr->chr_write = fd_chr_write;
    chr->chr_update_read_handler = fd_chr_update_read_handler;
    chr->chr_close = fd_chr_close;

    return chr;
}
