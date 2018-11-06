static int write_reftable_entry(BlockDriverState *bs, int rt_index)
{
    BDRVQcowState *s = bs->opaque;
    uint64_t buf[RT_ENTRIES_PER_SECTOR];
    int rt_start_index;
    int i, ret;

    rt_start_index = rt_index & ~(RT_ENTRIES_PER_SECTOR - 1);
    for (i = 0; i < RT_ENTRIES_PER_SECTOR; i++) {
        buf[i] = cpu_to_be64(s->refcount_table[rt_start_index + i]);
    }

    ret = qcow2_pre_write_overlap_check(bs,
            QCOW2_OL_DEFAULT & ~QCOW2_OL_REFCOUNT_TABLE,
            s->refcount_table_offset + rt_start_index * sizeof(uint64_t),
            sizeof(buf));
    if (ret < 0) {
        return ret;
    }

    BLKDBG_EVENT(bs->file, BLKDBG_REFTABLE_UPDATE);
    ret = bdrv_pwrite_sync(bs->file, s->refcount_table_offset +
            rt_start_index * sizeof(uint64_t), buf, sizeof(buf));
    if (ret < 0) {
        return ret;
    }

    return 0;
}
