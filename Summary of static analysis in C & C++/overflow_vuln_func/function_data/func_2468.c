static int qcow2_truncate(BlockDriverState *bs, int64_t offset)
{
    BDRVQcowState *s = bs->opaque;
    int ret, new_l1_size;

    if (offset & 511) {
        error_report("The new size must be a multiple of 512");
        return -EINVAL;
    }

    /
    if (s->nb_snapshots) {
        error_report("Can't resize an image which has snapshots");
        return -ENOTSUP;
    }

    /
    if (offset < bs->total_sectors * 512) {
        error_report("qcow2 doesn't support shrinking images yet");
        return -ENOTSUP;
    }

    new_l1_size = size_to_l1(s, offset);
    ret = qcow2_grow_l1_table(bs, new_l1_size, true);
    if (ret < 0) {
        return ret;
    }

    /
    offset = cpu_to_be64(offset);
    ret = bdrv_pwrite_sync(bs->file, offsetof(QCowHeader, size),
                           &offset, sizeof(uint64_t));
    if (ret < 0) {
        return ret;
    }

    s->l1_vm_state_index = new_l1_size;
    return 0;
}
