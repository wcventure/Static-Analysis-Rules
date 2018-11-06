static void http_log(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    
    if (logfile)
        vfprintf(logfile, fmt, ap);
    va_end(ap);
}
