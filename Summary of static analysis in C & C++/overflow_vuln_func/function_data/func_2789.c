static int sd_snapshot_goto(BlockDriverState *bs, const char *snapshot_id)
{
    BDRVSheepdogState *s = bs->opaque;
    BDRVSheepdogState *old_s;
    char vdi[SD_MAX_VDI_LEN], tag[SD_MAX_VDI_TAG_LEN];
    char *buf = NULL;
    uint32_t vid;
    uint32_t snapid = 0;
    int ret = 0, fd;

    old_s = g_malloc(sizeof(BDRVSheepdogState));

    memcpy(old_s, s, sizeof(BDRVSheepdogState));

    memset(vdi, 0, sizeof(vdi));
    strncpy(vdi, s->name, sizeof(vdi));

    memset(tag, 0, sizeof(tag));
    snapid = strtoul(snapshot_id, NULL, 10);
    if (!snapid) {
        strncpy(tag, s->name, sizeof(tag));
    }

    ret = find_vdi_name(s, vdi, snapid, tag, &vid, 1);
    if (ret) {
        error_report("Failed to find_vdi_name");
        goto out;
    }

    fd = connect_to_sdog(s->addr, s->port);
    if (fd < 0) {
        error_report("failed to connect");
        ret = fd;
        goto out;
    }

    buf = g_malloc(SD_INODE_SIZE);
    ret = read_object(fd, buf, vid_to_vdi_oid(vid), s->inode.nr_copies,
                      SD_INODE_SIZE, 0, s->cache_enabled);

    closesocket(fd);

    if (ret) {
        goto out;
    }

    memcpy(&s->inode, buf, sizeof(s->inode));

    if (!s->inode.vm_state_size) {
        error_report("Invalid snapshot");
        ret = -ENOENT;
        goto out;
    }

    s->is_snapshot = 1;

    g_free(buf);
    g_free(old_s);

    return 0;
out:
    /
    memcpy(s, old_s, sizeof(BDRVSheepdogState));
    g_free(buf);
    g_free(old_s);

    error_report("failed to open. recover old bdrv_sd_state.");

    return ret;
}
