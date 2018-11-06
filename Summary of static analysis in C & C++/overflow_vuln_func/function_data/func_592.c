static const char *
win32strerror(DWORD error)
{
    static char errbuf[ERRBUF_SIZE+1];
    size_t errlen;
    char *p;

    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		   NULL, error, 0, errbuf, ERRBUF_SIZE, NULL);

    /
    errlen = strlen(errbuf);
    if (errlen >= 2) {
        errbuf[errlen - 1] = '\0';
        errbuf[errlen - 2] = '\0';
    }
    p = strchr(errbuf, '\0');
    g_snprintf(p, (gulong)(sizeof errbuf - (p-errbuf)), " (%lu)", error);
    return errbuf;
}
