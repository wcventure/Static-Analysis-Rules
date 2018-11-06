static int sd_snapshot_goto(BlockDriverState *bs, const char *snapshot_id)
{
    BDRVSheepdogState *s = bs->opaque;
    BDRVSheepdogState *old_s;
    char tag[SD_MAX_VDI_TAG_LEN];
    uint32_t snapid = 0;
    int ret = 0;

    old_s = g_malloc(sizeof(BDRVSheepdogState));

    memcpy(old_s, s, sizeof(BDRVSheepdogState));

    snapid = strtoul(snapshot_id, NULL, 10);
    if (snapid) {
        tag[0] = 0;
    } else {
        pstrcpy(tag, sizeof(tag), snapshot_id);
    }

    ret = reload_inode(s, snapid, tag);
    if (ret) {
        goto out;
    }

    ret = sd_create_branch(s);
    if (ret) {
        goto out;
    }

    g_free(old_s);

    return 0;
out:
    /
    memcpy(s, old_s, sizeof(BDRVSheepdogState));
    g_free(old_s);

    error_report("failed to open. recover old bdrv_sd_state.");

    return ret;
}
