static CharDriverState *qemu_chr_open_pp_fd(int fd)
{
    CharDriverState *chr;
    ParallelCharDriver *drv;

    if (ioctl(fd, PPCLAIM) < 0) {
        close(fd);
        return NULL;
    }

    drv = g_malloc0(sizeof(ParallelCharDriver));
    drv->fd = fd;
    drv->mode = IEEE1284_MODE_COMPAT;

    chr = qemu_chr_alloc();
    chr->chr_write = null_chr_write;
    chr->chr_ioctl = pp_ioctl;
    chr->chr_close = pp_close;
    chr->opaque = drv;

    return chr;
}
