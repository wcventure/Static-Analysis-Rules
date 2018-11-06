static int qemu_peek_byte(QEMUFile *f)
{
    if (f->is_write) {
        abort();
    }

    if (f->buf_index >= f->buf_size) {
        qemu_fill_buffer(f);
        if (f->buf_index >= f->buf_size) {
            return 0;
        }
    }
    return f->buf[f->buf_index];
}
