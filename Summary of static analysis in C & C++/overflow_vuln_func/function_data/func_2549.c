QEMUFile *qemu_bufopen(const char *mode, QEMUSizedBuffer *input)
{
    QEMUBuffer *s;

    if (mode == NULL || (mode[0] != 'r' && mode[0] != 'w') ||
        mode[1] != '\0') {
        error_report("qemu_bufopen: Argument validity check failed");
        return NULL;
    }

    s = g_malloc0(sizeof(QEMUBuffer));
    s->qsb = input;

    if (s->qsb == NULL) {
        s->qsb = qsb_create(NULL, 0);
        s->qsb_allocated = true;
    }
    if (!s->qsb) {
        g_free(s);
        error_report("qemu_bufopen: qsb_create failed");
        return NULL;
    }


    if (mode[0] == 'r') {
        s->file = qemu_fopen_ops(s, &buf_read_ops);
    } else {
        s->file = qemu_fopen_ops(s, &buf_write_ops);
    }
    return s->file;
}
