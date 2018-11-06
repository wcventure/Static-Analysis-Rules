static int qcow2_cache_entry_flush(BlockDriverState *bs, Qcow2Cache *c, int i)
{
    BDRVQcowState *s = bs->opaque;
    int ret = 0;

    if (!c->entries[i].dirty || !c->entries[i].offset) {
        return 0;
    }

    trace_qcow2_cache_entry_flush(qemu_coroutine_self(),
                                  c == s->l2_table_cache, i);

    if (c->depends) {
        ret = qcow2_cache_flush_dependency(bs, c);
    } else if (c->depends_on_flush) {
        ret = bdrv_flush(bs->file);
        if (ret >= 0) {
            c->depends_on_flush = false;
        }
    }

    if (ret < 0) {
        return ret;
    }

    if (c == s->refcount_block_cache) {
        ret = qcow2_pre_write_overlap_check(bs,
                QCOW2_OL_DEFAULT & ~QCOW2_OL_REFCOUNT_BLOCK,
                c->entries[i].offset, s->cluster_size);
    } else if (c == s->l2_table_cache) {
        ret = qcow2_pre_write_overlap_check(bs,
                QCOW2_OL_DEFAULT & ~QCOW2_OL_ACTIVE_L2,
                c->entries[i].offset, s->cluster_size);
    } else {
        ret = qcow2_pre_write_overlap_check(bs, QCOW2_OL_DEFAULT,
                c->entries[i].offset, s->cluster_size);
    }

    if (ret < 0) {
        return ret;
    }

    if (c == s->refcount_block_cache) {
        BLKDBG_EVENT(bs->file, BLKDBG_REFBLOCK_UPDATE_PART);
    } else if (c == s->l2_table_cache) {
        BLKDBG_EVENT(bs->file, BLKDBG_L2_UPDATE);
    }

    ret = bdrv_pwrite(bs->file, c->entries[i].offset, c->entries[i].table,
        s->cluster_size);
    if (ret < 0) {
        return ret;
    }

    c->entries[i].dirty = false;

    return 0;
}
