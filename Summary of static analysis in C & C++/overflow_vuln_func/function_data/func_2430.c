static void balloon_stats_change_timer(VirtIOBalloon *s, int secs)
{
    timer_mod(s->stats_timer, qemu_clock_get_ms(QEMU_CLOCK_VIRTUAL) + secs * 1000);
}
