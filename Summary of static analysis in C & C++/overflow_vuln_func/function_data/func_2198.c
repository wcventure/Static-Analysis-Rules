static void GCC_FMT_ATTR(2, 3) qtest_send(CharDriverState *chr,
                                          const char *fmt, ...)
{
    va_list ap;
    char buffer[1024];
    size_t len;

    va_start(ap, fmt);
    len = vsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);

    qemu_chr_fe_write_all(chr, (uint8_t *)buffer, len);
    if (qtest_log_fp && qtest_opened) {
        fprintf(qtest_log_fp, "%s", buffer);
    }
}
