static int qemu_gluster_reopen_prepare(BDRVReopenState *state,
                                       BlockReopenQueue *queue, Error **errp)
{
    int ret = 0;
    BDRVGlusterReopenState *reop_s;
    GlusterConf *gconf = NULL;
    int open_flags = 0;

    assert(state != NULL);
    assert(state->bs != NULL);

    state->opaque = g_malloc0(sizeof(BDRVGlusterReopenState));
    reop_s = state->opaque;

    qemu_gluster_parse_flags(state->flags, &open_flags);

    gconf = g_malloc0(sizeof(GlusterConf));

    reop_s->glfs = qemu_gluster_init(gconf, state->bs->filename, errp);
    if (reop_s->glfs == NULL) {
        ret = -errno;
        goto exit;
    }

    reop_s->fd = glfs_open(reop_s->glfs, gconf->image, open_flags);
    if (reop_s->fd == NULL) {
        /
        ret = -errno;
        goto exit;
    }

exit:
    /
    qemu_gluster_gconf_free(gconf);
    return ret;
}
