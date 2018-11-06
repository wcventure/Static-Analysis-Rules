static int check_oflag_copied(BlockDriverState *bs, BdrvCheckResult *res,
                              BdrvCheckMode fix)
{
    BDRVQcowState *s = bs->opaque;
    uint64_t *l2_table = qemu_blockalign(bs, s->cluster_size);
    int ret;
    int refcount;
    int i, j;

    for (i = 0; i < s->l1_size; i++) {
        uint64_t l1_entry = s->l1_table[i];
        uint64_t l2_offset = l1_entry & L1E_OFFSET_MASK;
        bool l2_dirty = false;

        if (!l2_offset) {
            continue;
        }

        refcount = get_refcount(bs, l2_offset >> s->cluster_bits);
        if (refcount < 0) {
            /
            continue;
        }
        if ((refcount == 1) != ((l1_entry & QCOW_OFLAG_COPIED) != 0)) {
            fprintf(stderr, "%s OFLAG_COPIED L2 cluster: l1_index=%d "
                    "l1_entry=%" PRIx64 " refcount=%d\n",
                    fix & BDRV_FIX_ERRORS ? "Repairing" :
                                            "ERROR",
                    i, l1_entry, refcount);
            if (fix & BDRV_FIX_ERRORS) {
                s->l1_table[i] = refcount == 1
                               ? l1_entry |  QCOW_OFLAG_COPIED
                               : l1_entry & ~QCOW_OFLAG_COPIED;
                ret = qcow2_write_l1_entry(bs, i);
                if (ret < 0) {
                    res->check_errors++;
                    goto fail;
                }
                res->corruptions_fixed++;
            } else {
                res->corruptions++;
            }
        }

        ret = bdrv_pread(bs->file, l2_offset, l2_table,
                         s->l2_size * sizeof(uint64_t));
        if (ret < 0) {
            fprintf(stderr, "ERROR: Could not read L2 table: %s\n",
                    strerror(-ret));
            res->check_errors++;
            goto fail;
        }

        for (j = 0; j < s->l2_size; j++) {
            uint64_t l2_entry = be64_to_cpu(l2_table[j]);
            uint64_t data_offset = l2_entry & L2E_OFFSET_MASK;
            int cluster_type = qcow2_get_cluster_type(l2_entry);

            if ((cluster_type == QCOW2_CLUSTER_NORMAL) ||
                ((cluster_type == QCOW2_CLUSTER_ZERO) && (data_offset != 0))) {
                refcount = get_refcount(bs, data_offset >> s->cluster_bits);
                if (refcount < 0) {
                    /
                    continue;
                }
                if ((refcount == 1) != ((l2_entry & QCOW_OFLAG_COPIED) != 0)) {
                    fprintf(stderr, "%s OFLAG_COPIED data cluster: "
                            "l2_entry=%" PRIx64 " refcount=%d\n",
                            fix & BDRV_FIX_ERRORS ? "Repairing" :
                                                    "ERROR",
                            l2_entry, refcount);
                    if (fix & BDRV_FIX_ERRORS) {
                        l2_table[j] = cpu_to_be64(refcount == 1
                                    ? l2_entry |  QCOW_OFLAG_COPIED
                                    : l2_entry & ~QCOW_OFLAG_COPIED);
                        l2_dirty = true;
                        res->corruptions_fixed++;
                    } else {
                        res->corruptions++;
                    }
                }
            }
        }

        if (l2_dirty) {
            ret = qcow2_pre_write_overlap_check(bs,
                    QCOW2_OL_DEFAULT & ~QCOW2_OL_ACTIVE_L2, l2_offset,
                    s->cluster_size);
            if (ret < 0) {
                fprintf(stderr, "ERROR: Could not write L2 table; metadata "
                        "overlap check failed: %s\n", strerror(-ret));
                res->check_errors++;
                goto fail;
            }

            ret = bdrv_pwrite(bs->file, l2_offset, l2_table, s->cluster_size);
            if (ret < 0) {
                fprintf(stderr, "ERROR: Could not write L2 table: %s\n",
                        strerror(-ret));
                res->check_errors++;
                goto fail;
            }
        }
    }

    ret = 0;

fail:
    qemu_vfree(l2_table);
    return ret;
}
