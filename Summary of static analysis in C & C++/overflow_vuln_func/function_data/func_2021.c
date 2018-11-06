static void i6300esb_restart_timer(I6300State *d, int stage)
{
    int64_t timeout;

    if (!d->enabled)
        return;

    d->stage = stage;

    if (d->stage <= 1)
        timeout = d->timer1_preload;
    else
        timeout = d->timer2_preload;

    if (d->clock_scale == CLOCK_SCALE_1KHZ)
        timeout <<= 15;
    else
        timeout <<= 5;

    /
    timeout = muldiv64(get_ticks_per_sec(), timeout, 33000000);

    i6300esb_debug("stage %d, timeout %" PRIi64 "\n", d->stage, timeout);

    timer_mod(d->timer, qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) + timeout);
}
