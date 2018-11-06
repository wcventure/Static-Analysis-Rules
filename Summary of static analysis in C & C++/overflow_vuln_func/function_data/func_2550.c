QEMUFile *qemu_popen_cmd(const char *command, const char *mode)
{
    FILE *stdio_file;
    QEMUFileStdio *s;

    if (mode == NULL || (mode[0] != 'r' && mode[0] != 'w') || mode[1] != 0) {
        fprintf(stderr, "qemu_popen: Argument validity check failed\n");
        return NULL;
    }

    stdio_file = popen(command, mode);
    if (stdio_file == NULL) {
        return NULL;
    }

    s = g_malloc0(sizeof(QEMUFileStdio));

    s->stdio_file = stdio_file;

    if (mode[0] == 'r') {
        s->file = qemu_fopen_ops(s, &stdio_pipe_read_ops);
    } else {
        s->file = qemu_fopen_ops(s, &stdio_pipe_write_ops);
    }
    return s->file;
}
