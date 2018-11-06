static int alloc_refcount_block(BlockDriverState *bs,
    int64_t cluster_index, uint16_t **refcount_block)
{
    BDRVQcowState *s = bs->opaque;
    unsigned int refcount_table_index;
    int ret;

    BLKDBG_EVENT(bs->file, BLKDBG_REFBLOCK_ALLOC);

    /
    refcount_table_index = cluster_index >> (s->cluster_bits - REFCOUNT_SHIFT);

    if (refcount_table_index < s->refcount_table_size) {

        uint64_t refcount_block_offset =
            s->refcount_table[refcount_table_index] & REFT_OFFSET_MASK;

        /
        if (refcount_block_offset) {
             return load_refcount_block(bs, refcount_block_offset,
                 (void**) refcount_block);
        }
    }

    /

    *refcount_block = NULL;

    /
    ret = qcow2_cache_flush(bs, s->l2_table_cache);
    if (ret < 0) {
        return ret;
    }

    /
    int64_t new_block = alloc_clusters_noref(bs, s->cluster_size);
    if (new_block < 0) {
        return new_block;
    }

#ifdef DEBUG_ALLOC2
    fprintf(stderr, "qcow2: Allocate refcount block %d for %" PRIx64
        " at %" PRIx64 "\n",
        refcount_table_index, cluster_index << s->cluster_bits, new_block);
#endif

    if (in_same_refcount_block(s, new_block, cluster_index << s->cluster_bits)) {
        /
        ret = qcow2_cache_get_empty(bs, s->refcount_block_cache, new_block,
            (void**) refcount_block);
        if (ret < 0) {
            goto fail_block;
        }

        memset(*refcount_block, 0, s->cluster_size);

        /
        int block_index = (new_block >> s->cluster_bits) &
            ((1 << (s->cluster_bits - REFCOUNT_SHIFT)) - 1);
        (*refcount_block)[block_index] = cpu_to_be16(1);
    } else {
        /
        ret = update_refcount(bs, new_block, s->cluster_size, 1,
                              QCOW2_DISCARD_NEVER);
        if (ret < 0) {
            goto fail_block;
        }

        ret = qcow2_cache_flush(bs, s->refcount_block_cache);
        if (ret < 0) {
            goto fail_block;
        }

        /
        ret = qcow2_cache_get_empty(bs, s->refcount_block_cache, new_block,
            (void**) refcount_block);
        if (ret < 0) {
            goto fail_block;
        }

        memset(*refcount_block, 0, s->cluster_size);
    }

    /
    BLKDBG_EVENT(bs->file, BLKDBG_REFBLOCK_ALLOC_WRITE);
    qcow2_cache_entry_mark_dirty(s->refcount_block_cache, *refcount_block);
    ret = qcow2_cache_flush(bs, s->refcount_block_cache);
    if (ret < 0) {
        goto fail_block;
    }

    /
    if (refcount_table_index < s->refcount_table_size) {
        uint64_t data64 = cpu_to_be64(new_block);
        BLKDBG_EVENT(bs->file, BLKDBG_REFBLOCK_ALLOC_HOOKUP);
        ret = bdrv_pwrite_sync(bs->file,
            s->refcount_table_offset + refcount_table_index * sizeof(uint64_t),
            &data64, sizeof(data64));
        if (ret < 0) {
            goto fail_block;
        }

        s->refcount_table[refcount_table_index] = new_block;

        /
        return -EAGAIN;
    }

    ret = qcow2_cache_put(bs, s->refcount_block_cache, (void**) refcount_block);
    if (ret < 0) {
        goto fail_block;
    }

    /
    BLKDBG_EVENT(bs->file, BLKDBG_REFTABLE_GROW);

    /
    uint64_t refcount_block_clusters = 1 << (s->cluster_bits - REFCOUNT_SHIFT);
    uint64_t blocks_used = DIV_ROUND_UP(cluster_index, refcount_block_clusters);

    if (blocks_used > QCOW_MAX_REFTABLE_SIZE / sizeof(uint64_t)) {
        return -EFBIG;
    }

    /
    uint64_t table_size = next_refcount_table_size(s, blocks_used + 1);
    uint64_t last_table_size;
    uint64_t blocks_clusters;
    do {
        uint64_t table_clusters =
            size_to_clusters(s, table_size * sizeof(uint64_t));
        blocks_clusters = 1 +
            ((table_clusters + refcount_block_clusters - 1)
            / refcount_block_clusters);
        uint64_t meta_clusters = table_clusters + blocks_clusters;

        last_table_size = table_size;
        table_size = next_refcount_table_size(s, blocks_used +
            ((meta_clusters + refcount_block_clusters - 1)
            / refcount_block_clusters));

    } while (last_table_size != table_size);

#ifdef DEBUG_ALLOC2
    fprintf(stderr, "qcow2: Grow refcount table %" PRId32 " => %" PRId64 "\n",
        s->refcount_table_size, table_size);
#endif

    /
    uint64_t meta_offset = (blocks_used * refcount_block_clusters) *
        s->cluster_size;
    uint64_t table_offset = meta_offset + blocks_clusters * s->cluster_size;
    uint64_t *new_table = g_try_malloc0(table_size * sizeof(uint64_t));
    uint16_t *new_blocks = g_try_malloc0(blocks_clusters * s->cluster_size);

    assert(table_size > 0 && blocks_clusters > 0);
    if (new_table == NULL || new_blocks == NULL) {
        ret = -ENOMEM;
        goto fail_table;
    }

    /
    memcpy(new_table, s->refcount_table,
        s->refcount_table_size * sizeof(uint64_t));
    new_table[refcount_table_index] = new_block;

    int i;
    for (i = 0; i < blocks_clusters; i++) {
        new_table[blocks_used + i] = meta_offset + (i * s->cluster_size);
    }

    /
    uint64_t table_clusters = size_to_clusters(s, table_size * sizeof(uint64_t));
    int block = 0;
    for (i = 0; i < table_clusters + blocks_clusters; i++) {
        new_blocks[block++] = cpu_to_be16(1);
    }

    /
    BLKDBG_EVENT(bs->file, BLKDBG_REFBLOCK_ALLOC_WRITE_BLOCKS);
    ret = bdrv_pwrite_sync(bs->file, meta_offset, new_blocks,
        blocks_clusters * s->cluster_size);
    g_free(new_blocks);
    new_blocks = NULL;
    if (ret < 0) {
        goto fail_table;
    }

    /
    for(i = 0; i < table_size; i++) {
        cpu_to_be64s(&new_table[i]);
    }

    BLKDBG_EVENT(bs->file, BLKDBG_REFBLOCK_ALLOC_WRITE_TABLE);
    ret = bdrv_pwrite_sync(bs->file, table_offset, new_table,
        table_size * sizeof(uint64_t));
    if (ret < 0) {
        goto fail_table;
    }

    for(i = 0; i < table_size; i++) {
        be64_to_cpus(&new_table[i]);
    }

    /
    uint8_t data[12];
    cpu_to_be64w((uint64_t*)data, table_offset);
    cpu_to_be32w((uint32_t*)(data + 8), table_clusters);
    BLKDBG_EVENT(bs->file, BLKDBG_REFBLOCK_ALLOC_SWITCH_TABLE);
    ret = bdrv_pwrite_sync(bs->file, offsetof(QCowHeader, refcount_table_offset),
        data, sizeof(data));
    if (ret < 0) {
        goto fail_table;
    }

    /
    uint64_t old_table_offset = s->refcount_table_offset;
    uint64_t old_table_size = s->refcount_table_size;

    g_free(s->refcount_table);
    s->refcount_table = new_table;
    s->refcount_table_size = table_size;
    s->refcount_table_offset = table_offset;

    /
    qcow2_free_clusters(bs, old_table_offset, old_table_size * sizeof(uint64_t),
                        QCOW2_DISCARD_OTHER);

    ret = load_refcount_block(bs, new_block, (void**) refcount_block);
    if (ret < 0) {
        return ret;
    }

    /
    return -EAGAIN;

fail_table:
    g_free(new_blocks);
    g_free(new_table);
fail_block:
    if (*refcount_block != NULL) {
        qcow2_cache_put(bs, s->refcount_block_cache, (void**) refcount_block);
    }
    return ret;
}
