static CharDriverState *qemu_chr_open_pipe(ChardevHostdev *opts)
{
    const char *filename = opts->device;
    CharDriverState *chr;
    WinCharState *s;

    chr = qemu_chr_alloc();
    s = g_malloc0(sizeof(WinCharState));
    chr->opaque = s;
    chr->chr_write = win_chr_write;
    chr->chr_close = win_chr_close;

    if (win_chr_pipe_init(chr, filename) < 0) {
        g_free(s);
        g_free(chr);
        return NULL;
    }
    return chr;
}
