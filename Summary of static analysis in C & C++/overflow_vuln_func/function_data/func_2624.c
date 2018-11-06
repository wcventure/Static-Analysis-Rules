void monitor_flush(Monitor *mon)
{
    int rc;

    if (mon && mon->outbuf_index != 0 && !mon->mux_out) {
        rc = qemu_chr_fe_write(mon->chr, mon->outbuf, mon->outbuf_index);
        if (rc == mon->outbuf_index) {
            /
            mon->outbuf_index = 0;
            return;
        }
        if (rc > 0) {
            /
            memmove(mon->outbuf, mon->outbuf + rc, mon->outbuf_index - rc);
            mon->outbuf_index -= rc;
        }
        qemu_chr_fe_add_watch(mon->chr, G_IO_OUT, monitor_unblocked, mon);
    }
}
