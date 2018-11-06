void monitor_vprintf(Monitor *mon, const char *fmt, va_list ap)
{
    char buf[4096];

    if (!mon)
        return;

    if (monitor_ctrl_mode(mon)) {
        return;
    }

    vsnprintf(buf, sizeof(buf), fmt, ap);
    monitor_puts(mon, buf);
}
