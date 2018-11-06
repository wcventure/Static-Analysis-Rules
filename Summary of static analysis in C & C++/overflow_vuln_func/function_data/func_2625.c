static void monitor_puts(Monitor *mon, const char *str)
{
    char c;

    for(;;) {
        assert(mon->outbuf_index < sizeof(mon->outbuf) - 1);
        c = *str++;
        if (c == '\0')
            break;
        if (c == '\n')
            mon->outbuf[mon->outbuf_index++] = '\r';
        mon->outbuf[mon->outbuf_index++] = c;
        if (mon->outbuf_index >= (sizeof(mon->outbuf) - 1)
            || c == '\n')
            monitor_flush(mon);
    }
}
