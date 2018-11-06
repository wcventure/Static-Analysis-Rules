void av_log_format_line(void *ptr, int level, const char *fmt, va_list vl,
                        char *line, int line_size, int *print_prefix)
{
    char part[3][512];
    format_line(ptr, level, fmt, vl, part, sizeof(part[0]), print_prefix, NULL);
    snprintf(line, line_size, "%s%s%s", part[0], part[1], part[2]);
}
