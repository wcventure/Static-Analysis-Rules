static int tcp_set_msgfds(CharDriverState *chr, int *fds, int num)
{
    TCPCharDriver *s = chr->opaque;

    /
    g_free(s->write_msgfds);

    if (num) {
        s->write_msgfds = g_malloc(num * sizeof(int));
        memcpy(s->write_msgfds, fds, num * sizeof(int));
    }

    s->write_msgfds_num = num;

    return 0;
}
