static TAPState *net_tap_fd_init(VLANState *vlan, int fd)
{
    TAPState *s;

    s = qemu_mallocz(sizeof(TAPState));
    if (!s)
        return NULL;
    s->fd = fd;
    s->vc = qemu_new_vlan_client(vlan, tap_receive, s);
    qemu_set_fd_handler(s->fd, tap_send, NULL, s);
    snprintf(s->vc->info_str, sizeof(s->vc->info_str), "tap: fd=%d", fd);
    return s;
}
