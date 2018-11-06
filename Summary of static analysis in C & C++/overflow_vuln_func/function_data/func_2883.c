static void nbd_reply_ready(void *opaque)
{
    BDRVNBDState *s = opaque;
    int i;

    if (s->reply.handle == 0) {
        /
        if (nbd_receive_reply(s->sock, &s->reply) < 0) {
            s->reply.handle = 0;
            goto fail;
        }
    }

    /
    i = HANDLE_TO_INDEX(s, s->reply.handle);
    if (s->recv_coroutine[i]) {
        qemu_coroutine_enter(s->recv_coroutine[i], NULL);
        return;
    }

fail:
    for (i = 0; i < MAX_NBD_REQUESTS; i++) {
        if (s->recv_coroutine[i]) {
            qemu_coroutine_enter(s->recv_coroutine[i], NULL);
        }
    }
}
