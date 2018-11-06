static gchar *
get_net_error_message(gint error)
{
    HMODULE module = NULL;
    gchar *retval = NULL;
    wchar_t *msg = NULL;
    int flags;
    size_t nchars;

    flags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_IGNORE_INSERTS |
        FORMAT_MESSAGE_FROM_SYSTEM;

    if (error >= NERR_BASE && error <= MAX_NERR) {
        module = LoadLibraryExW(L"netmsg.dll", NULL, LOAD_LIBRARY_AS_DATAFILE);

        if (module != NULL) {
            flags |= FORMAT_MESSAGE_FROM_HMODULE;
        }
    }

    FormatMessageW(flags, module, error, 0, (LPWSTR)&msg, 0, NULL);

    if (msg != NULL) {
        nchars = wcslen(msg);

        if (nchars > 2 &&
            msg[nchars - 1] == L'\n' &&
            msg[nchars - 2] == L'\r') {
            msg[nchars - 2] = L'\0';
        }

        retval = g_utf16_to_utf8(msg, -1, NULL, NULL, NULL);

        LocalFree(msg);
    }

    if (module != NULL) {
        FreeLibrary(module);
    }

    return retval;
}
