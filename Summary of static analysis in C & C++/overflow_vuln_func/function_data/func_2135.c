static struct glfs *qemu_gluster_init(BlockdevOptionsGluster *gconf,
                                      const char *filename,
                                      QDict *options, Error **errp)
{
    int ret;
    if (filename) {
        ret = qemu_gluster_parse_uri(gconf, filename);
        if (ret < 0) {
            error_setg(errp, "invalid URI");
            error_append_hint(errp, "Usage: file=gluster[+transport]://"
                                    "[host[:port]]/volume/path[?socket=...]\n");
            errno = -ret;
            return NULL;
        }
    } else {
        ret = qemu_gluster_parse_json(gconf, options, errp);
        if (ret < 0) {
            error_append_hint(errp, "Usage: "
                             "-drive driver=qcow2,file.driver=gluster,"
                             "file.volume=testvol,file.path=/path/a.qcow2"
                             "[,file.debug=9],file.server.0.type=tcp,"
                             "file.server.0.host=1.2.3.4,"
                             "file.server.0.port=24007,"
                             "file.server.1.transport=unix,"
                             "file.server.1.socket=/var/run/glusterd.socket ..."
                             "\n");
            errno = -ret;
            return NULL;
        }

    }

    return qemu_gluster_glfs_init(gconf, errp);
}
