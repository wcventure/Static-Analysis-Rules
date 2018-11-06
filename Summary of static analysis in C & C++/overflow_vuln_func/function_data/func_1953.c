static int buffered_rate_limit(void *opaque)
{
    MigrationState *s = opaque;
    int ret;

    ret = qemu_file_get_error(s->file);
    if (ret) {
        return ret;
    }

    if (s->bytes_xfer > s->xfer_limit) {
        return 1;
    }

    return 0;
}
