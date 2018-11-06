static int raw_reopen_prepare(BDRVReopenState *state,
                              BlockReopenQueue *queue, Error **errp)
{
    BDRVRawState *s;
    BDRVRawReopenState *raw_s;
    int ret = 0;
    Error *local_err = NULL;

    assert(state != NULL);
    assert(state->bs != NULL);

    s = state->bs->opaque;

    state->opaque = g_malloc0(sizeof(BDRVRawReopenState));
    raw_s = state->opaque;

#ifdef CONFIG_LINUX_AIO
    raw_s->use_aio = s->use_aio;

    /
    if (raw_set_aio(&s->aio_ctx, &raw_s->use_aio, state->flags)) {
        error_setg(errp, "Could not set AIO state");
        return -1;
    }
#endif

    if (s->type == FTYPE_FD || s->type == FTYPE_CD) {
        raw_s->open_flags |= O_NONBLOCK;
    }

    raw_parse_flags(state->flags, &raw_s->open_flags);

    raw_s->fd = -1;

    int fcntl_flags = O_APPEND | O_NONBLOCK;
#ifdef O_NOATIME
    fcntl_flags |= O_NOATIME;
#endif

#ifdef O_ASYNC
    /
    assert((s->open_flags & O_ASYNC) == 0);
#endif

    if ((raw_s->open_flags & ~fcntl_flags) == (s->open_flags & ~fcntl_flags)) {
        /
        /
#ifdef F_DUPFD_CLOEXEC
        raw_s->fd = fcntl(s->fd, F_DUPFD_CLOEXEC, 0);
#else
        raw_s->fd = dup(s->fd);
        if (raw_s->fd != -1) {
            qemu_set_cloexec(raw_s->fd);
        }
#endif
        if (raw_s->fd >= 0) {
            ret = fcntl_setfl(raw_s->fd, raw_s->open_flags);
            if (ret) {
                qemu_close(raw_s->fd);
                raw_s->fd = -1;
            }
        }
    }

    /
    if (raw_s->fd == -1) {
        assert(!(raw_s->open_flags & O_CREAT));
        raw_s->fd = qemu_open(state->bs->filename, raw_s->open_flags);
        if (raw_s->fd == -1) {
            error_setg_errno(errp, errno, "Could not reopen file");
            ret = -1;
        }
    }

    /
    if (raw_s->fd != -1) {
        raw_probe_alignment(state->bs, raw_s->fd, &local_err);
        if (local_err) {
            qemu_close(raw_s->fd);
            raw_s->fd = -1;
            error_propagate(errp, local_err);
            ret = -EINVAL;
        }
    }

    return ret;
}
