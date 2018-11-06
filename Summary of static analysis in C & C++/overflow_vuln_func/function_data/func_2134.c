static struct glfs *qemu_gluster_glfs_init(BlockdevOptionsGluster *gconf,
                                           Error **errp)
{
    struct glfs *glfs;
    int ret;
    int old_errno;
    GlusterServerList *server;

    glfs = glfs_new(gconf->volume);
    if (!glfs) {
        goto out;
    }

    for (server = gconf->server; server; server = server->next) {
        if (server->value->type  == GLUSTER_TRANSPORT_UNIX) {
            ret = glfs_set_volfile_server(glfs,
                                   GlusterTransport_lookup[server->value->type],
                                   server->value->u.q_unix.path, 0);
        } else {
            ret = glfs_set_volfile_server(glfs,
                                   GlusterTransport_lookup[server->value->type],
                                   server->value->u.tcp.host,
                                   atoi(server->value->u.tcp.port));
        }

        if (ret < 0) {
            goto out;
        }
    }

    ret = glfs_set_logging(glfs, "-", gconf->debug_level);
    if (ret < 0) {
        goto out;
    }

    ret = glfs_init(glfs);
    if (ret) {
        error_setg(errp, "Gluster connection for volume %s, path %s failed"
                         " to connect", gconf->volume, gconf->path);
        for (server = gconf->server; server; server = server->next) {
            if (server->value->type  == GLUSTER_TRANSPORT_UNIX) {
                error_append_hint(errp, "hint: failed on socket %s ",
                                  server->value->u.q_unix.path);
            } else {
                error_append_hint(errp, "hint: failed on host %s and port %s ",
                                  server->value->u.tcp.host,
                                  server->value->u.tcp.port);
            }
        }

        error_append_hint(errp, "Please refer to gluster logs for more info\n");

        /
        if (errno == 0) {
            errno = EINVAL;
        }

        goto out;
    }
    return glfs;

out:
    if (glfs) {
        old_errno = errno;
        glfs_fini(glfs);
        errno = old_errno;
    }
    return NULL;
}
