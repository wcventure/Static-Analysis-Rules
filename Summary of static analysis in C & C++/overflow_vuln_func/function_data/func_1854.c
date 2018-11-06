int v9fs_device_realize_common(V9fsState *s, Error **errp)
{
    int i, len;
    struct stat stat;
    FsDriverEntry *fse;
    V9fsPath path;
    int rc = 1;

    /
    QLIST_INIT(&s->free_list);
    QLIST_INIT(&s->active_list);
    for (i = 0; i < (MAX_REQ - 1); i++) {
        QLIST_INSERT_HEAD(&s->free_list, &s->pdus[i], next);
        s->pdus[i].s = s;
        s->pdus[i].idx = i;
    }

    v9fs_path_init(&path);

    fse = get_fsdev_fsentry(s->fsconf.fsdev_id);

    if (!fse) {
        /
        error_setg(errp, "9pfs device couldn't find fsdev with the "
                   "id = %s",
                   s->fsconf.fsdev_id ? s->fsconf.fsdev_id : "NULL");
        goto out;
    }

    if (!s->fsconf.tag) {
        /
        error_setg(errp, "fsdev with id %s needs mount_tag arguments",
                   s->fsconf.fsdev_id);
        goto out;
    }

    s->ctx.export_flags = fse->export_flags;
    s->ctx.fs_root = g_strdup(fse->path);
    s->ctx.exops.get_st_gen = NULL;
    len = strlen(s->fsconf.tag);
    if (len > MAX_TAG_LEN - 1) {
        error_setg(errp, "mount tag '%s' (%d bytes) is longer than "
                   "maximum (%d bytes)", s->fsconf.tag, len, MAX_TAG_LEN - 1);
        goto out;
    }

    s->tag = g_strdup(s->fsconf.tag);
    s->ctx.uid = -1;

    s->ops = fse->ops;

    s->fid_list = NULL;
    qemu_co_rwlock_init(&s->rename_lock);

    if (s->ops->init(&s->ctx) < 0) {
        error_setg(errp, "9pfs Failed to initialize fs-driver with id:%s"
                   " and export path:%s", s->fsconf.fsdev_id, s->ctx.fs_root);
        goto out;
    }

    /
    if (s->ops->name_to_path(&s->ctx, NULL, "/", &path) < 0) {
        error_setg(errp,
                   "error in converting name to path %s", strerror(errno));
        goto out;
    }
    if (s->ops->lstat(&s->ctx, &path, &stat)) {
        error_setg(errp, "share path %s does not exist", fse->path);
        goto out;
    } else if (!S_ISDIR(stat.st_mode)) {
        error_setg(errp, "share path %s is not a directory", fse->path);
        goto out;
    }
    v9fs_path_free(&path);

    rc = 0;
out:
    if (rc) {
        if (s->ops && s->ops->cleanup && s->ctx.private) {
            s->ops->cleanup(&s->ctx);
        }
        g_free(s->tag);
        g_free(s->ctx.fs_root);
        v9fs_path_free(&path);
    }
    return rc;
}
