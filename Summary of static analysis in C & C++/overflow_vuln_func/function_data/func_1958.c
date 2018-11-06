static int expand_zero_clusters_in_l1(BlockDriverState *bs, uint64_t *l1_table,
                                      int l1_size, uint8_t **expanded_clusters,
                                      uint64_t *nb_clusters)
{
    BDRVQcowState *s = bs->opaque;
    bool is_active_l1 = (l1_table == s->l1_table);
    uint64_t *l2_table = NULL;
    int ret;
    int i, j;

    if (!is_active_l1) {
        /
        l2_table = qemu_blockalign(bs, s->cluster_size);
    }

    for (i = 0; i < l1_size; i++) {
        uint64_t l2_offset = l1_table[i] & L1E_OFFSET_MASK;
        bool l2_dirty = false;

        if (!l2_offset) {
            /
            continue;
        }

        if (is_active_l1) {
            /
            ret = qcow2_cache_get(bs, s->l2_table_cache, l2_offset,
                    (void **)&l2_table);
        } else {
            /
            ret = bdrv_read(bs->file, l2_offset / BDRV_SECTOR_SIZE,
                    (void *)l2_table, s->cluster_sectors);
        }
        if (ret < 0) {
            goto fail;
        }

        for (j = 0; j < s->l2_size; j++) {
            uint64_t l2_entry = be64_to_cpu(l2_table[j]);
            int64_t offset = l2_entry & L2E_OFFSET_MASK, cluster_index;
            int cluster_type = qcow2_get_cluster_type(l2_entry);
            bool preallocated = offset != 0;

            if (cluster_type == QCOW2_CLUSTER_NORMAL) {
                cluster_index = offset >> s->cluster_bits;
                assert((cluster_index >= 0) && (cluster_index < *nb_clusters));
                if ((*expanded_clusters)[cluster_index / 8] &
                    (1 << (cluster_index % 8))) {
                    /
                    ret = qcow2_update_cluster_refcount(bs, cluster_index, 1,
                                                        QCOW2_DISCARD_NEVER);
                    if (ret < 0) {
                        goto fail;
                    }
                    /
                    l2_table[j] = cpu_to_be64(l2_entry & ~QCOW_OFLAG_COPIED);
                    l2_dirty = true;
                }
                continue;
            }
            else if (qcow2_get_cluster_type(l2_entry) != QCOW2_CLUSTER_ZERO) {
                continue;
            }

            if (!preallocated) {
                if (!bs->backing_hd) {
                    /
                    l2_table[j] = 0;
                    l2_dirty = true;
                    continue;
                }

                offset = qcow2_alloc_clusters(bs, s->cluster_size);
                if (offset < 0) {
                    ret = offset;
                    goto fail;
                }
            }

            ret = qcow2_pre_write_overlap_check(bs, QCOW2_OL_DEFAULT,
                                                offset, s->cluster_size);
            if (ret < 0) {
                if (!preallocated) {
                    qcow2_free_clusters(bs, offset, s->cluster_size,
                                        QCOW2_DISCARD_ALWAYS);
                }
                goto fail;
            }

            ret = bdrv_write_zeroes(bs->file, offset / BDRV_SECTOR_SIZE,
                                    s->cluster_sectors);
            if (ret < 0) {
                if (!preallocated) {
                    qcow2_free_clusters(bs, offset, s->cluster_size,
                                        QCOW2_DISCARD_ALWAYS);
                }
                goto fail;
            }

            l2_table[j] = cpu_to_be64(offset | QCOW_OFLAG_COPIED);
            l2_dirty = true;

            cluster_index = offset >> s->cluster_bits;

            if (cluster_index >= *nb_clusters) {
                uint64_t old_bitmap_size = (*nb_clusters + 7) / 8;
                uint64_t new_bitmap_size;
                /
                assert(bs->file->growable);
                *nb_clusters = size_to_clusters(s, bs->file->total_sectors *
                                                BDRV_SECTOR_SIZE);
                new_bitmap_size = (*nb_clusters + 7) / 8;
                *expanded_clusters = g_realloc(*expanded_clusters,
                                               new_bitmap_size);
                /
                memset(&(*expanded_clusters)[old_bitmap_size], 0,
                       new_bitmap_size - old_bitmap_size);
            }

            assert((cluster_index >= 0) && (cluster_index < *nb_clusters));
            (*expanded_clusters)[cluster_index / 8] |= 1 << (cluster_index % 8);
        }

        if (is_active_l1) {
            if (l2_dirty) {
                qcow2_cache_entry_mark_dirty(s->l2_table_cache, l2_table);
                qcow2_cache_depends_on_flush(s->l2_table_cache);
            }
            ret = qcow2_cache_put(bs, s->l2_table_cache, (void **)&l2_table);
            if (ret < 0) {
                l2_table = NULL;
                goto fail;
            }
        } else {
            if (l2_dirty) {
                ret = qcow2_pre_write_overlap_check(bs, QCOW2_OL_DEFAULT &
                        ~(QCOW2_OL_INACTIVE_L2 | QCOW2_OL_ACTIVE_L2), l2_offset,
                        s->cluster_size);
                if (ret < 0) {
                    goto fail;
                }

                ret = bdrv_write(bs->file, l2_offset / BDRV_SECTOR_SIZE,
                        (void *)l2_table, s->cluster_sectors);
                if (ret < 0) {
                    goto fail;
                }
            }
        }
    }

    ret = 0;

fail:
    if (l2_table) {
        if (!is_active_l1) {
            qemu_vfree(l2_table);
        } else {
            if (ret < 0) {
                qcow2_cache_put(bs, s->l2_table_cache, (void **)&l2_table);
            } else {
                ret = qcow2_cache_put(bs, s->l2_table_cache,
                        (void **)&l2_table);
            }
        }
    }
    return ret;
}
