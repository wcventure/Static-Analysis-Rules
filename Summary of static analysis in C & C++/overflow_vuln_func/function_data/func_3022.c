static CharDriverState *qmp_chardev_open_socket(ChardevSocket *sock,
                                                Error **errp)
{
    CharDriverState *chr;
    TCPCharDriver *s;
    SocketAddress *addr = sock->addr;
    bool do_nodelay     = sock->has_nodelay ? sock->nodelay : false;
    bool is_listen      = sock->has_server  ? sock->server  : true;
    bool is_telnet      = sock->has_telnet  ? sock->telnet  : false;
    bool is_waitconnect = sock->has_wait    ? sock->wait    : false;
    int64_t reconnect   = sock->has_reconnect ? sock->reconnect : 0;

    chr = qemu_chr_alloc();
    s = g_malloc0(sizeof(TCPCharDriver));

    s->fd = -1;
    s->listen_fd = -1;
    s->is_unix = addr->kind == SOCKET_ADDRESS_KIND_UNIX;
    s->is_listen = is_listen;
    s->is_telnet = is_telnet;
    s->do_nodelay = do_nodelay;
    qapi_copy_SocketAddress(&s->addr, sock->addr);

    chr->opaque = s;
    chr->chr_write = tcp_chr_write;
    chr->chr_sync_read = tcp_chr_sync_read;
    chr->chr_close = tcp_chr_close;
    chr->get_msgfds = tcp_get_msgfds;
    chr->set_msgfds = tcp_set_msgfds;
    chr->chr_add_client = tcp_chr_add_client;
    chr->chr_add_watch = tcp_chr_add_watch;
    chr->chr_update_read_handler = tcp_chr_update_read_handler;
    /
    chr->explicit_be_open = true;

    chr->filename = g_malloc(CHR_MAX_FILENAME_SIZE);
    SocketAddress_to_str(chr->filename, CHR_MAX_FILENAME_SIZE, "disconnected:",
                         addr, is_listen, is_telnet);

    if (is_listen) {
        if (is_telnet) {
            s->do_telnetopt = 1;
        }
    } else if (reconnect > 0) {
        s->reconnect_time = reconnect;
    }

    if (s->reconnect_time) {
        socket_try_connect(chr);
    } else if (!qemu_chr_open_socket_fd(chr, errp)) {
        g_free(s);
        g_free(chr->filename);
        g_free(chr);
        return NULL;
    }

    if (is_listen && is_waitconnect) {
        fprintf(stderr, "QEMU waiting for connection on: %s\n",
                chr->filename);
        tcp_chr_accept(s->listen_chan, G_IO_IN, chr);
        qemu_set_nonblock(s->listen_fd);
    }

    return chr;
}
