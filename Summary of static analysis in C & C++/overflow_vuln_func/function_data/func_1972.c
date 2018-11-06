static int vmdk_open_desc_file(BlockDriverState *bs, int flags,
                               int64_t desc_offset)
{
    int ret;
    char *buf = NULL;
    char ct[128];
    BDRVVmdkState *s = bs->opaque;
    int64_t size;

    size = bdrv_getlength(bs->file);
    if (size < 0) {
        return -EINVAL;
    }

    size = MIN(size, 1 << 20);  /
    buf = g_malloc0(size + 1);

    ret = bdrv_pread(bs->file, desc_offset, buf, size);
    if (ret < 0) {
        goto exit;
    }
    if (vmdk_parse_description(buf, "createType", ct, sizeof(ct))) {
        ret = -EMEDIUMTYPE;
        goto exit;
    }
    if (strcmp(ct, "monolithicFlat") &&
        strcmp(ct, "twoGbMaxExtentSparse") &&
        strcmp(ct, "twoGbMaxExtentFlat")) {
        fprintf(stderr,
                "VMDK: Not supported image type \"%s\""".\n", ct);
        ret = -ENOTSUP;
        goto exit;
    }
    s->desc_offset = 0;
    ret = vmdk_parse_extents(buf, bs, bs->file->filename);
exit:
    g_free(buf);
    return ret;
}
