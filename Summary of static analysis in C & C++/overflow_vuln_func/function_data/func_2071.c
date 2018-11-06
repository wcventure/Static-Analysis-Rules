static void balloon_stats_set_poll_interval(Object *obj, struct Visitor *v,
                                            void *opaque, const char *name,
                                            Error **errp)
{
    VirtIOBalloon *s = opaque;
    Error *local_err = NULL;
    int64_t value;

    visit_type_int(v, &value, name, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        return;
    }

    if (value < 0) {
        error_setg(errp, "timer value must be greater than zero");
        return;
    }

    if (value > UINT_MAX) {
        error_setg(errp, "timer value is too big");
        return;
    }

    if (value == s->stats_poll_interval) {
        return;
    }

    if (value == 0) {
        /
        balloon_stats_destroy_timer(s);
        return;
    }

    if (balloon_stats_enabled(s)) {
        /
        s->stats_poll_interval = value;
        balloon_stats_change_timer(s, value);
        return;
    }

    /
    g_assert(s->stats_timer == NULL);
    s->stats_timer = timer_new_ms(QEMU_CLOCK_VIRTUAL, balloon_stats_poll_cb, s);
    s->stats_poll_interval = value;
    balloon_stats_change_timer(s, 0);
}
