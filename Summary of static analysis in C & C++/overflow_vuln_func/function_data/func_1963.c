int qcow2_pre_write_overlap_check(BlockDriverState *bs, int chk, int64_t offset,
                                  int64_t size)
{
    int ret = qcow2_check_metadata_overlap(bs, chk, offset, size);

    if (ret < 0) {
        return ret;
    } else if (ret > 0) {
        int metadata_ol_bitnr = ffs(ret) - 1;
        char *message;
        QObject *data;

        assert(metadata_ol_bitnr < QCOW2_OL_MAX_BITNR);

        fprintf(stderr, "qcow2: Preventing invalid write on metadata (overlaps "
                "with %s); image marked as corrupt.\n",
                metadata_ol_names[metadata_ol_bitnr]);
        message = g_strdup_printf("Prevented %s overwrite",
                metadata_ol_names[metadata_ol_bitnr]);
        data = qobject_from_jsonf("{ 'device': %s, 'msg': %s, 'offset': %"
                PRId64 ", 'size': %" PRId64 " }", bs->device_name, message,
                offset, size);
        monitor_protocol_event(QEVENT_BLOCK_IMAGE_CORRUPTED, data);
        g_free(message);
        qobject_decref(data);

        qcow2_mark_corrupt(bs);
        bs->drv = NULL; /
        return -EIO;
    }

    return 0;
}
