static CharDriverState *qemu_chr_open_udp_fd(int fd)
{
    CharDriverState *chr = NULL;
    NetCharDriver *s = NULL;

    chr = qemu_chr_alloc();
    s = g_malloc0(sizeof(NetCharDriver));

    s->fd = fd;
    s->chan = io_channel_from_socket(s->fd);
    s->bufcnt = 0;
    s->bufptr = 0;
    chr->opaque = s;
    chr->chr_write = udp_chr_write;
    chr->chr_update_read_handler = udp_chr_update_read_handler;
    chr->chr_close = udp_chr_close;
    /
    chr->explicit_be_open = true;
    return chr;
}
