static int vhdx_log_flush(BlockDriverState *bs, BDRVVHDXState *s,
                          VHDXLogSequence *logs)
{
    int ret = 0;
    int i;
    uint32_t cnt, sectors_read;
    uint64_t new_file_size;
    void *data = NULL;
    int64_t file_length;
    VHDXLogDescEntries *desc_entries = NULL;
    VHDXLogEntryHeader hdr_tmp = { 0 };

    cnt = logs->count;

    data = qemu_blockalign(bs, VHDX_LOG_SECTOR_SIZE);

    ret = vhdx_user_visible_write(bs, s);
    if (ret < 0) {
        goto exit;
    }

    /
    while (cnt--) {
        ret = vhdx_log_peek_hdr(bs, &logs->log, &hdr_tmp);
        if (ret < 0) {
            goto exit;
        }
        file_length = bdrv_getlength(bs->file->bs);
        if (file_length < 0) {
            ret = file_length;
            goto exit;
        }
        /
        if (hdr_tmp.flushed_file_offset > file_length) {
            ret = -EINVAL;
            goto exit;
        }

        ret = vhdx_log_read_desc(bs, s, &logs->log, &desc_entries, true);
        if (ret < 0) {
            goto exit;
        }

        for (i = 0; i < desc_entries->hdr.descriptor_count; i++) {
            if (desc_entries->desc[i].signature == VHDX_LOG_DESC_SIGNATURE) {
                /
                ret = vhdx_log_read_sectors(bs, &logs->log, &sectors_read,
                                            data, 1, false);
                if (ret < 0) {
                    goto exit;
                }
                if (sectors_read != 1) {
                    ret = -EINVAL;
                    goto exit;
                }
                vhdx_log_data_le_import(data);
            }

            ret = vhdx_log_flush_desc(bs, &desc_entries->desc[i], data);
            if (ret < 0) {
                goto exit;
            }
        }
        if (file_length < desc_entries->hdr.last_file_offset) {
            new_file_size = desc_entries->hdr.last_file_offset;
            if (new_file_size % (1024*1024)) {
                /
                new_file_size = ((new_file_size >> 20) + 1) << 20;
                bdrv_truncate(bs->file, new_file_size, PREALLOC_MODE_OFF, NULL);
            }
        }
        qemu_vfree(desc_entries);
        desc_entries = NULL;
    }

    bdrv_flush(bs);
    /
    vhdx_log_reset(bs, s);

exit:
    qemu_vfree(data);
    qemu_vfree(desc_entries);
    return ret;
}
