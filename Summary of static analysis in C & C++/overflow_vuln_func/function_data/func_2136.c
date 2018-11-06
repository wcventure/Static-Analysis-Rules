static int qemu_gluster_open(BlockDriverState *bs,  QDict *options,
                             int bdrv_flags, Error **errp)
{
    BDRVGlusterState *s = bs->opaque;
    int open_flags = 0;
    int ret = 0;
    BlockdevOptionsGluster *gconf = NULL;
    QemuOpts *opts;
    Error *local_err = NULL;
    const char *filename;

    opts = qemu_opts_create(&runtime_opts, NULL, 0, &error_abort);
    qemu_opts_absorb_qdict(opts, options, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        ret = -EINVAL;
        goto out;
    }

    filename = qemu_opt_get(opts, GLUSTER_OPT_FILENAME);

    s->debug_level = qemu_opt_get_number(opts, GLUSTER_OPT_DEBUG,
                                         GLUSTER_DEBUG_DEFAULT);
    if (s->debug_level < 0) {
        s->debug_level = 0;
    } else if (s->debug_level > GLUSTER_DEBUG_MAX) {
        s->debug_level = GLUSTER_DEBUG_MAX;
    }

    gconf = g_new0(BlockdevOptionsGluster, 1);
    gconf->debug_level = s->debug_level;
    gconf->has_debug_level = true;
    s->glfs = qemu_gluster_init(gconf, filename, options, errp);
    if (!s->glfs) {
        ret = -errno;
        goto out;
    }

#ifdef CONFIG_GLUSTERFS_XLATOR_OPT
    /
    ret = glfs_set_xlator_option(s->glfs, "*-write-behind",
                                          "resync-failed-syncs-after-fsync",
                                          "on");
    if (ret < 0) {
        error_setg_errno(errp, errno, "Unable to set xlator key/value pair");
        ret = -errno;
        goto out;
    }
#endif

    qemu_gluster_parse_flags(bdrv_flags, &open_flags);

    s->fd = glfs_open(s->glfs, gconf->path, open_flags);
    if (!s->fd) {
        ret = -errno;
    }

    s->supports_seek_data = qemu_gluster_test_seek(s->fd);

out:
    qemu_opts_del(opts);
    qapi_free_BlockdevOptionsGluster(gconf);
    if (!ret) {
        return ret;
    }
    if (s->fd) {
        glfs_close(s->fd);
    }
    if (s->glfs) {
        glfs_fini(s->glfs);
    }
    return ret;
}
