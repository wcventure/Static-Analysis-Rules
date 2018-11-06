static NetSocketState *net_socket_fd_init_stream(VLANState *vlan, int fd, 
                                          int is_connected)
{
    NetSocketState *s;
    s = qemu_mallocz(sizeof(NetSocketState));
    if (!s)
        return NULL;
    s->fd = fd;
    s->vc = qemu_new_vlan_client(vlan, 
                                 net_socket_receive, s);
    snprintf(s->vc->info_str, sizeof(s->vc->info_str),
             "socket: fd=%d", fd);
    if (is_connected) {
        net_socket_connect(s);
    } else {
        qemu_set_fd_handler(s->fd, NULL, net_socket_connect, s);
    }
    return s;
}
