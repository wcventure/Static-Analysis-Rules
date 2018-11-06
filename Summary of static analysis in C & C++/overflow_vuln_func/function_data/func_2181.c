static int64_t nfs_client_open(NFSClient *client, QDict *options,
                               int flags, int open_flags, Error **errp)
{
    int ret = -EINVAL;
    QemuOpts *opts = NULL;
    Error *local_err = NULL;
    struct stat st;
    char *file = NULL, *strp = NULL;

    qemu_mutex_init(&client->mutex);
    opts = qemu_opts_create(&runtime_opts, NULL, 0, &error_abort);
    qemu_opts_absorb_qdict(opts, options, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        ret = -EINVAL;
        goto fail;
    }

    client->path = g_strdup(qemu_opt_get(opts, "path"));
    if (!client->path) {
        ret = -EINVAL;
        error_setg(errp, "No path was specified");
        goto fail;
    }

    strp = strrchr(client->path, '/');
    if (strp == NULL) {
        error_setg(errp, "Invalid URL specified");
        goto fail;
    }
    file = g_strdup(strp);
    *strp = 0;

    /
    client->server = nfs_config(options, errp);
    if (!client->server) {
        ret = -EINVAL;
        goto fail;
    }

    client->context = nfs_init_context();
    if (client->context == NULL) {
        error_setg(errp, "Failed to init NFS context");
        goto fail;
    }

    if (qemu_opt_get(opts, "user")) {
        client->uid = qemu_opt_get_number(opts, "user", 0);
        nfs_set_uid(client->context, client->uid);
    }

    if (qemu_opt_get(opts, "group")) {
        client->gid = qemu_opt_get_number(opts, "group", 0);
        nfs_set_gid(client->context, client->gid);
    }

    if (qemu_opt_get(opts, "tcp-syn-count")) {
        client->tcp_syncnt = qemu_opt_get_number(opts, "tcp-syn-count", 0);
        nfs_set_tcp_syncnt(client->context, client->tcp_syncnt);
    }

#ifdef LIBNFS_FEATURE_READAHEAD
    if (qemu_opt_get(opts, "readahead-size")) {
        if (open_flags & BDRV_O_NOCACHE) {
            error_setg(errp, "Cannot enable NFS readahead "
                             "if cache.direct = on");
            goto fail;
        }
        client->readahead = qemu_opt_get_number(opts, "readahead-size", 0);
        if (client->readahead > QEMU_NFS_MAX_READAHEAD_SIZE) {
            warn_report("Truncating NFS readahead size to %d",
                        QEMU_NFS_MAX_READAHEAD_SIZE);
            client->readahead = QEMU_NFS_MAX_READAHEAD_SIZE;
        }
        nfs_set_readahead(client->context, client->readahead);
#ifdef LIBNFS_FEATURE_PAGECACHE
        nfs_set_pagecache_ttl(client->context, 0);
#endif
        client->cache_used = true;
    }
#endif

#ifdef LIBNFS_FEATURE_PAGECACHE
    if (qemu_opt_get(opts, "page-cache-size")) {
        if (open_flags & BDRV_O_NOCACHE) {
            error_setg(errp, "Cannot enable NFS pagecache "
                             "if cache.direct = on");
            goto fail;
        }
        client->pagecache = qemu_opt_get_number(opts, "page-cache-size", 0);
        if (client->pagecache > QEMU_NFS_MAX_PAGECACHE_SIZE) {
            warn_report("Truncating NFS pagecache size to %d pages",
                        QEMU_NFS_MAX_PAGECACHE_SIZE);
            client->pagecache = QEMU_NFS_MAX_PAGECACHE_SIZE;
        }
        nfs_set_pagecache(client->context, client->pagecache);
        nfs_set_pagecache_ttl(client->context, 0);
        client->cache_used = true;
    }
#endif

#ifdef LIBNFS_FEATURE_DEBUG
    if (qemu_opt_get(opts, "debug")) {
        client->debug = qemu_opt_get_number(opts, "debug", 0);
        /
        if (client->debug > QEMU_NFS_MAX_DEBUG_LEVEL) {
            warn_report("Limiting NFS debug level to %d",
                        QEMU_NFS_MAX_DEBUG_LEVEL);
            client->debug = QEMU_NFS_MAX_DEBUG_LEVEL;
        }
        nfs_set_debug(client->context, client->debug);
    }
#endif

    ret = nfs_mount(client->context, client->server->host, client->path);
    if (ret < 0) {
        error_setg(errp, "Failed to mount nfs share: %s",
                   nfs_get_error(client->context));
        goto fail;
    }

    if (flags & O_CREAT) {
        ret = nfs_creat(client->context, file, 0600, &client->fh);
        if (ret < 0) {
            error_setg(errp, "Failed to create file: %s",
                       nfs_get_error(client->context));
            goto fail;
        }
    } else {
        ret = nfs_open(client->context, file, flags, &client->fh);
        if (ret < 0) {
            error_setg(errp, "Failed to open file : %s",
                       nfs_get_error(client->context));
            goto fail;
        }
    }

    ret = nfs_fstat(client->context, client->fh, &st);
    if (ret < 0) {
        error_setg(errp, "Failed to fstat file: %s",
                   nfs_get_error(client->context));
        goto fail;
    }

    ret = DIV_ROUND_UP(st.st_size, BDRV_SECTOR_SIZE);
    client->st_blocks = st.st_blocks;
    client->has_zero_init = S_ISREG(st.st_mode);
    *strp = '/';
    goto out;

fail:
    nfs_client_close(client);
out:
    qemu_opts_del(opts);
    g_free(file);
    return ret;
}
