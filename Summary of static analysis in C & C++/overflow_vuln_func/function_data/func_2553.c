QEMUFile *qemu_fopen_socket(int fd, const char *mode)
{
    QEMUFileSocket *s;

    if (qemu_file_mode_is_not_valid(mode)) {
        return NULL;
    }

    s = g_malloc0(sizeof(QEMUFileSocket));
    s->fd = fd;
    if (mode[0] == 'w') {
        qemu_set_block(s->fd);
        s->file = qemu_fopen_ops(s, &socket_write_ops);
    } else {
        s->file = qemu_fopen_ops(s, &socket_read_ops);
    }
    return s->file;
}
