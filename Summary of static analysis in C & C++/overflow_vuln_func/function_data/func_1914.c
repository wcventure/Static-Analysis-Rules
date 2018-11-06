static int hdev_get_max_segments(const struct stat *st)
{
#ifdef CONFIG_LINUX
    char buf[32];
    const char *end;
    char *sysfspath;
    int ret;
    int fd = -1;
    long max_segments;

    sysfspath = g_strdup_printf("/sys/dev/block/%u:%u/queue/max_segments",
                                major(st->st_rdev), minor(st->st_rdev));
    fd = open(sysfspath, O_RDONLY);
    if (fd == -1) {
        ret = -errno;
        goto out;
    }
    do {
        ret = read(fd, buf, sizeof(buf));
    } while (ret == -1 && errno == EINTR);
    if (ret < 0) {
        ret = -errno;
        goto out;
    } else if (ret == 0) {
        ret = -EIO;
        goto out;
    }
    buf[ret] = 0;
    /
    ret = qemu_strtol(buf, &end, 10, &max_segments);
    if (ret == 0 && end && *end == '\n') {
        ret = max_segments;
    }

out:
    g_free(sysfspath);
    return ret;
#else
    return -ENOTSUP;
#endif
}
