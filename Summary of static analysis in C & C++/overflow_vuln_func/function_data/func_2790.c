static int sd_snapshot_list(BlockDriverState *bs, QEMUSnapshotInfo **psn_tab)
{
    BDRVSheepdogState *s = bs->opaque;
    SheepdogReq req;
    int fd, nr = 1024, ret, max = BITS_TO_LONGS(SD_NR_VDIS) * sizeof(long);
    QEMUSnapshotInfo *sn_tab = NULL;
    unsigned wlen, rlen;
    int found = 0;
    static SheepdogInode inode;
    unsigned long *vdi_inuse;
    unsigned int start_nr;
    uint64_t hval;
    uint32_t vid;

    vdi_inuse = g_malloc(max);

    fd = connect_to_sdog(s->addr, s->port);
    if (fd < 0) {
        ret = fd;
        goto out;
    }

    rlen = max;
    wlen = 0;

    memset(&req, 0, sizeof(req));

    req.opcode = SD_OP_READ_VDIS;
    req.data_length = max;

    ret = do_req(fd, (SheepdogReq *)&req, vdi_inuse, &wlen, &rlen);

    closesocket(fd);
    if (ret) {
        goto out;
    }

    sn_tab = g_malloc0(nr * sizeof(*sn_tab));

    /
    hval = fnv_64a_buf(s->name, strlen(s->name), FNV1A_64_INIT);
    start_nr = hval & (SD_NR_VDIS - 1);

    fd = connect_to_sdog(s->addr, s->port);
    if (fd < 0) {
        error_report("failed to connect");
        ret = fd;
        goto out;
    }

    for (vid = start_nr; found < nr; vid = (vid + 1) % SD_NR_VDIS) {
        if (!test_bit(vid, vdi_inuse)) {
            break;
        }

        /
        ret = read_object(fd, (char *)&inode, vid_to_vdi_oid(vid),
                          0, SD_INODE_SIZE - sizeof(inode.data_vdi_id), 0,
                          s->cache_enabled);

        if (ret) {
            continue;
        }

        if (!strcmp(inode.name, s->name) && is_snapshot(&inode)) {
            sn_tab[found].date_sec = inode.snap_ctime >> 32;
            sn_tab[found].date_nsec = inode.snap_ctime & 0xffffffff;
            sn_tab[found].vm_state_size = inode.vm_state_size;
            sn_tab[found].vm_clock_nsec = inode.vm_clock_nsec;

            snprintf(sn_tab[found].id_str, sizeof(sn_tab[found].id_str), "%u",
                     inode.snap_id);
            strncpy(sn_tab[found].name, inode.tag,
                    MIN(sizeof(sn_tab[found].name), sizeof(inode.tag)));
            found++;
        }
    }

    closesocket(fd);
out:
    *psn_tab = sn_tab;

    g_free(vdi_inuse);

    if (ret < 0) {
        return ret;
    }

    return found;
}
