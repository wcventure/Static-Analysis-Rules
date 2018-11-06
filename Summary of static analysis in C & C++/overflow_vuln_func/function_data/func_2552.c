QEMUFile *qemu_fdopen(int fd, const char *mode)
{
    QEMUFileSocket *s;

    if (mode == NULL ||
        (mode[0] != 'r' && mode[0] != 'w') ||
        mode[1] != 'b' || mode[2] != 0) {
        fprintf(stderr, "qemu_fdopen: Argument validity check failed\n");
        return NULL;
    }

    s = g_malloc0(sizeof(QEMUFileSocket));
    s->fd = fd;

    if (mode[0] == 'r') {
        s->file = qemu_fopen_ops(s, &unix_read_ops);
    } else {
        s->file = qemu_fopen_ops(s, &unix_write_ops);
    }
    return s->file;
}
