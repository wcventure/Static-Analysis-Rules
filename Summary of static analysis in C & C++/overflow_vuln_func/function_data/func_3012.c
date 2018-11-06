static CharDriverState *qemu_chr_open_mux(CharDriverState *drv)
{
    CharDriverState *chr;
    MuxDriver *d;

    chr = qemu_chr_alloc();
    d = g_malloc0(sizeof(MuxDriver));

    chr->opaque = d;
    d->drv = drv;
    d->focus = -1;
    chr->chr_write = mux_chr_write;
    chr->chr_update_read_handler = mux_chr_update_read_handler;
    chr->chr_accept_input = mux_chr_accept_input;
    /
    chr->chr_set_fe_open = NULL;
    if (drv->chr_add_watch) {
        chr->chr_add_watch = mux_chr_add_watch;
    }
    /
    chr->explicit_be_open = muxes_realized ? 0 : 1;
    chr->is_mux = 1;

    return chr;
}
