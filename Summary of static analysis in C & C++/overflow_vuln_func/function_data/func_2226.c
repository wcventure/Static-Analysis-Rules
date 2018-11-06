bool aio_poll(AioContext *ctx, bool blocking)
{
    AioHandler *node;
    HANDLE events[MAXIMUM_WAIT_OBJECTS + 1];
    bool was_dispatching, progress, have_select_revents, first;
    int count;
    int timeout;

    have_select_revents = aio_prepare(ctx);
    if (have_select_revents) {
        blocking = false;
    }

    was_dispatching = ctx->dispatching;
    progress = false;

    /
    aio_set_dispatching(ctx, !blocking);

    ctx->walking_handlers++;

    /
    count = 0;
    QLIST_FOREACH(node, &ctx->aio_handlers, node) {
        if (!node->deleted && node->io_notify) {
            events[count++] = event_notifier_get_handle(node->e);
        }
    }

    ctx->walking_handlers--;
    first = true;

    /
    while (count > 0) {
        HANDLE event;
        int ret;

        timeout = blocking
            ? qemu_timeout_ns_to_ms(aio_compute_timeout(ctx)) : 0;
        ret = WaitForMultipleObjects(count, events, FALSE, timeout);
        aio_set_dispatching(ctx, true);

        if (first && aio_bh_poll(ctx)) {
            progress = true;
        }
        first = false;

        /
        event = NULL;
        if ((DWORD) (ret - WAIT_OBJECT_0) < count) {
            event = events[ret - WAIT_OBJECT_0];
        } else if (!have_select_revents) {
            break;
        }

        have_select_revents = false;
        blocking = false;

        progress |= aio_dispatch_handlers(ctx, event);

        /
        events[ret - WAIT_OBJECT_0] = events[--count];
    }

    progress |= timerlistgroup_run_timers(&ctx->tlg);

    aio_set_dispatching(ctx, was_dispatching);
    return progress;
}
