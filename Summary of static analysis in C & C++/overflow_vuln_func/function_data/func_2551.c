QEMUFile *qemu_fopen(const char *filename, const char *mode)
{
    QEMUFileStdio *s;

    if (qemu_file_mode_is_not_valid(mode)) {
        return NULL;
    }

    s = g_malloc0(sizeof(QEMUFileStdio));

    s->stdio_file = fopen(filename, mode);
    if (!s->stdio_file) {
        goto fail;
    }

    if (mode[0] == 'w') {
        s->file = qemu_fopen_ops(s, &stdio_file_write_ops);
    } else {
        s->file = qemu_fopen_ops(s, &stdio_file_read_ops);
    }
    return s->file;
fail:
    g_free(s);
    return NULL;
}
