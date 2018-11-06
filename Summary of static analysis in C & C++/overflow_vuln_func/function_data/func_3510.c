void av_log_default_callback(void* ptr, int level, const char* fmt, va_list vl)
{
    static int print_prefix = 1;
    static int count;
    static char prev[1024];
    char part[3][512];
    char line[1024];
    static int is_atty;
    int type[2];

    if (level > av_log_level)
        return;
    format_line(ptr, level, fmt, vl, part, sizeof(part[0]), &print_prefix, type);
    snprintf(line, sizeof(line), "%s%s%s", part[0], part[1], part[2]);

#if HAVE_ISATTY
    if (!is_atty)
        is_atty = isatty(2) ? 1 : -1;
#endif

#undef fprintf
    if (print_prefix && (flags & AV_LOG_SKIP_REPEATED) && !strcmp(line, prev)){
        count++;
        if (is_atty == 1)
            fprintf(stderr, "    Last message repeated %d times\r", count);
        return;
    }
    if (count > 0) {
        fprintf(stderr, "    Last message repeated %d times\n", count);
        count = 0;
    }
    strcpy(prev, line);
    sanitize(part[0]);
    colored_fputs(type[0], part[0]);
    sanitize(part[1]);
    colored_fputs(type[1], part[1]);
    sanitize(part[2]);
    colored_fputs(av_clip(level >> 3, 0, 6), part[2]);
}
