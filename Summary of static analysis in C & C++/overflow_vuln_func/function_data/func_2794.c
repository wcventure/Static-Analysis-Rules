int qemu_get_buffer(QEMUFile *f, uint8_t *buf, int size1)
{
    int size, l;

    if (f->is_write) {
        abort();
    }

    size = size1;
    while (size > 0) {
        l = f->buf_size - f->buf_index;
        if (l == 0) {
            qemu_fill_buffer(f);
            l = f->buf_size - f->buf_index;
            if (l == 0) {
                break;
            }
        }
        if (l > size) {
            l = size;
        }
        memcpy(buf, f->buf + f->buf_index, l);
        f->buf_index += l;
        buf += l;
        size -= l;
    }
    return size1 - size;
}
