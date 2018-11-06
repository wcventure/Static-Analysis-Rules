static int sd_create(const char *filename, QemuOpts *opts,
                     Error **errp)
{
    int ret = 0;
    uint32_t vid = 0;
    char *backing_file = NULL;
    char *buf = NULL;
    BDRVSheepdogState *s;
    char tag[SD_MAX_VDI_TAG_LEN];
    uint32_t snapid;
    bool prealloc = false;

    s = g_malloc0(sizeof(BDRVSheepdogState));

    memset(tag, 0, sizeof(tag));
    if (strstr(filename, "://")) {
        ret = sd_parse_uri(s, filename, s->name, &snapid, tag);
    } else {
        ret = parse_vdiname(s, filename, s->name, &snapid, tag);
    }
    if (ret < 0) {
        error_setg(errp, "Can't parse filename");
        goto out;
    }

    s->inode.vdi_size = qemu_opt_get_size_del(opts, BLOCK_OPT_SIZE, 0);
    backing_file = qemu_opt_get_del(opts, BLOCK_OPT_BACKING_FILE);
    buf = qemu_opt_get_del(opts, BLOCK_OPT_PREALLOC);
    if (!buf || !strcmp(buf, "off")) {
        prealloc = false;
    } else if (!strcmp(buf, "full")) {
        prealloc = true;
    } else {
        error_setg(errp, "Invalid preallocation mode: '%s'", buf);
        ret = -EINVAL;
        goto out;
    }

    g_free(buf);
    buf = qemu_opt_get_del(opts, BLOCK_OPT_REDUNDANCY);
    if (buf) {
        ret = parse_redundancy(s, buf);
        if (ret < 0) {
            error_setg(errp, "Invalid redundancy mode: '%s'", buf);
            goto out;
        }
    }

    if (s->inode.vdi_size > SD_MAX_VDI_SIZE) {
        error_setg(errp, "too big image size");
        ret = -EINVAL;
        goto out;
    }

    if (backing_file) {
        BlockDriverState *bs;
        BDRVSheepdogState *base;
        BlockDriver *drv;

        /
        drv = bdrv_find_protocol(backing_file, true);
        if (!drv || strcmp(drv->protocol_name, "sheepdog") != 0) {
            error_setg(errp, "backing_file must be a sheepdog image");
            ret = -EINVAL;
            goto out;
        }

        bs = NULL;
        ret = bdrv_open(&bs, backing_file, NULL, NULL, BDRV_O_PROTOCOL, NULL,
                        errp);
        if (ret < 0) {
            goto out;
        }

        base = bs->opaque;

        if (!is_snapshot(&base->inode)) {
            error_setg(errp, "cannot clone from a non snapshot vdi");
            bdrv_unref(bs);
            ret = -EINVAL;
            goto out;
        }
        s->inode.vdi_id = base->inode.vdi_id;
        bdrv_unref(bs);
    }

    s->aio_context = qemu_get_aio_context();
    ret = do_sd_create(s, &vid, 0, errp);
    if (ret) {
        goto out;
    }

    if (prealloc) {
        ret = sd_prealloc(filename, errp);
    }
out:
    g_free(backing_file);
    g_free(buf);
    g_free(s);
    return ret;
}
