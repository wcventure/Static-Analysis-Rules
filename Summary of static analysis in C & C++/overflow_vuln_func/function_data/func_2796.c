int qemu_get_byte(QEMUFile *f)
{
    int result;

    result = qemu_peek_byte(f);

    if (f->buf_index < f->buf_size) {
        f->buf_index++;
    }
    return result;
}
