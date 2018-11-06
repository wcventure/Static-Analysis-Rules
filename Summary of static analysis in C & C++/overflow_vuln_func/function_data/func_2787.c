static int sd_open(BlockDriverState *bs, const char *filename, int flags)
{
    int ret, fd;
    uint32_t vid = 0;
    BDRVSheepdogState *s = bs->opaque;
    char vdi[SD_MAX_VDI_LEN], tag[SD_MAX_VDI_TAG_LEN];
    uint32_t snapid;
    char *buf = NULL;

    strstart(filename, "sheepdog:", (const char **)&filename);

    QLIST_INIT(&s->inflight_aio_head);
    QLIST_INIT(&s->pending_aio_head);
    s->fd = -1;

    memset(vdi, 0, sizeof(vdi));
    memset(tag, 0, sizeof(tag));
    if (parse_vdiname(s, filename, vdi, &snapid, tag) < 0) {
        ret = -EINVAL;
        goto out;
    }
    s->fd = get_sheep_fd(s);
    if (s->fd < 0) {
        ret = s->fd;
        goto out;
    }

    ret = find_vdi_name(s, vdi, snapid, tag, &vid, 0);
    if (ret) {
        goto out;
    }

    s->cache_enabled = 1;
    s->flush_fd = connect_to_sdog(s->addr, s->port);
    if (s->flush_fd < 0) {
        error_report("failed to connect");
        ret = s->flush_fd;
        goto out;
    }

    if (snapid || tag[0] != '\0') {
        dprintf("%" PRIx32 " snapshot inode was open.\n", vid);
        s->is_snapshot = 1;
    }

    fd = connect_to_sdog(s->addr, s->port);
    if (fd < 0) {
        error_report("failed to connect");
        ret = fd;
        goto out;
    }

    buf = g_malloc(SD_INODE_SIZE);
    ret = read_object(fd, buf, vid_to_vdi_oid(vid), 0, SD_INODE_SIZE, 0,
                      s->cache_enabled);

    closesocket(fd);

    if (ret) {
        goto out;
    }

    memcpy(&s->inode, buf, sizeof(s->inode));
    s->min_dirty_data_idx = UINT32_MAX;
    s->max_dirty_data_idx = 0;

    bs->total_sectors = s->inode.vdi_size / SECTOR_SIZE;
    strncpy(s->name, vdi, sizeof(s->name));
    qemu_co_mutex_init(&s->lock);
    g_free(buf);
    return 0;
out:
    qemu_aio_set_fd_handler(s->fd, NULL, NULL, NULL, NULL);
    if (s->fd >= 0) {
        closesocket(s->fd);
    }
    g_free(buf);
    return ret;
}
