static int qemu_gluster_create(const char *filename,
                               QemuOpts *opts, Error **errp)
{
    struct glfs *glfs;
    struct glfs_fd *fd;
    int ret = 0;
    int prealloc = 0;
    int64_t total_size = 0;
    char *tmp = NULL;
    GlusterConf *gconf = g_malloc0(sizeof(GlusterConf));

    glfs = qemu_gluster_init(gconf, filename, errp);
    if (!glfs) {
        ret = -errno;
        goto out;
    }

    total_size =
        qemu_opt_get_size_del(opts, BLOCK_OPT_SIZE, 0) / BDRV_SECTOR_SIZE;

    tmp = qemu_opt_get_del(opts, BLOCK_OPT_PREALLOC);
    if (!tmp || !strcmp(tmp, "off")) {
        prealloc = 0;
    } else if (!strcmp(tmp, "full") &&
               gluster_supports_zerofill()) {
        prealloc = 1;
    } else {
        error_setg(errp, "Invalid preallocation mode: '%s'"
            " or GlusterFS doesn't support zerofill API",
            tmp);
        ret = -EINVAL;
        goto out;
    }

    fd = glfs_creat(glfs, gconf->image,
        O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, S_IRUSR | S_IWUSR);
    if (!fd) {
        ret = -errno;
    } else {
        if (!glfs_ftruncate(fd, total_size * BDRV_SECTOR_SIZE)) {
            if (prealloc && qemu_gluster_zerofill(fd, 0,
                    total_size * BDRV_SECTOR_SIZE)) {
                ret = -errno;
            }
        } else {
            ret = -errno;
        }

        if (glfs_close(fd) != 0) {
            ret = -errno;
        }
    }
out:
    g_free(tmp);
    qemu_gluster_gconf_free(gconf);
    if (glfs) {
        glfs_fini(glfs);
    }
    return ret;
}
