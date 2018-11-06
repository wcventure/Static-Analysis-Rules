static int http_send_data(HTTPContext *c)
{
    int len, ret;

    while (c->buffer_ptr >= c->buffer_end) {
        ret = http_prepare_data(c);
        if (ret < 0)
            return -1;
        else if (ret == 0) {
            break;
        } else {
            /
            return 0;
        }
    }

    if (c->buffer_end > c->buffer_ptr) {
        len = write(c->fd, c->buffer_ptr, c->buffer_end - c->buffer_ptr);
        if (len < 0) {
            if (errno != EAGAIN && errno != EINTR) {
                /
                return -1;
            }
        } else {
            c->buffer_ptr += len;
            c->data_count += len;
        }
    }
    return 0;
}
