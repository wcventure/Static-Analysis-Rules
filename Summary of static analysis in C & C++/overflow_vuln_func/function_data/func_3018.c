static CharDriverState *qemu_chr_open_win_file(HANDLE fd_out)
{
    CharDriverState *chr;
    WinCharState *s;

    chr = qemu_chr_alloc();
    s = g_malloc0(sizeof(WinCharState));
    s->hcom = fd_out;
    chr->opaque = s;
    chr->chr_write = win_chr_write;
    return chr;
}
