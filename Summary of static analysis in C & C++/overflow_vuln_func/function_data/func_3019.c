static CharDriverState *qemu_chr_open_stdio(ChardevStdio *opts)
{
    CharDriverState   *chr;
    WinStdioCharState *stdio;
    DWORD              dwMode;
    int                is_console = 0;

    chr   = qemu_chr_alloc();
    stdio = g_malloc0(sizeof(WinStdioCharState));

    stdio->hStdIn = GetStdHandle(STD_INPUT_HANDLE);
    if (stdio->hStdIn == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "cannot open stdio: invalid handle\n");
        exit(1);
    }

    is_console = GetConsoleMode(stdio->hStdIn, &dwMode) != 0;

    chr->opaque    = stdio;
    chr->chr_write = win_stdio_write;
    chr->chr_close = win_stdio_close;

    if (is_console) {
        if (qemu_add_wait_object(stdio->hStdIn,
                                 win_stdio_wait_func, chr)) {
            fprintf(stderr, "qemu_add_wait_object: failed\n");
        }
    } else {
        DWORD   dwId;
            
        stdio->hInputReadyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        stdio->hInputDoneEvent  = CreateEvent(NULL, FALSE, FALSE, NULL);
        stdio->hInputThread     = CreateThread(NULL, 0, win_stdio_thread,
                                               chr, 0, &dwId);

        if (stdio->hInputThread == INVALID_HANDLE_VALUE
            || stdio->hInputReadyEvent == INVALID_HANDLE_VALUE
            || stdio->hInputDoneEvent == INVALID_HANDLE_VALUE) {
            fprintf(stderr, "cannot create stdio thread or event\n");
            exit(1);
        }
        if (qemu_add_wait_object(stdio->hInputReadyEvent,
                                 win_stdio_thread_wait_func, chr)) {
            fprintf(stderr, "qemu_add_wait_object: failed\n");
        }
    }

    dwMode |= ENABLE_LINE_INPUT;

    if (is_console) {
        /
        /
        dwMode |= ENABLE_PROCESSED_INPUT;
    }

    SetConsoleMode(stdio->hStdIn, dwMode);

    chr->chr_set_echo = qemu_chr_set_echo_win_stdio;
    qemu_chr_fe_set_echo(chr, false);

    return chr;
}
