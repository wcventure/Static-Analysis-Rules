static BlockMeasureInfo *qcow2_measure(QemuOpts *opts, BlockDriverState *in_bs,
                                       Error **errp)
{
    Error *local_err = NULL;
    BlockMeasureInfo *info;
    uint64_t required = 0; /
    uint64_t virtual_size; /
    uint64_t refcount_bits;
    uint64_t l2_tables;
    size_t cluster_size;
    int version;
    char *optstr;
    PreallocMode prealloc;
    bool has_backing_file;

    /
    cluster_size = qcow2_opt_get_cluster_size_del(opts, &local_err);
    if (local_err) {
        goto err;
    }

    version = qcow2_opt_get_version_del(opts, &local_err);
    if (local_err) {
        goto err;
    }

    refcount_bits = qcow2_opt_get_refcount_bits_del(opts, version, &local_err);
    if (local_err) {
        goto err;
    }

    optstr = qemu_opt_get_del(opts, BLOCK_OPT_PREALLOC);
    prealloc = qapi_enum_parse(PreallocMode_lookup, optstr,
                               PREALLOC_MODE__MAX, PREALLOC_MODE_OFF,
                               &local_err);
    g_free(optstr);
    if (local_err) {
        goto err;
    }

    optstr = qemu_opt_get_del(opts, BLOCK_OPT_BACKING_FILE);
    has_backing_file = !!optstr;
    g_free(optstr);

    virtual_size = align_offset(qemu_opt_get_size_del(opts, BLOCK_OPT_SIZE, 0),
                                cluster_size);

    /
    l2_tables = DIV_ROUND_UP(virtual_size / cluster_size,
                             cluster_size / sizeof(uint64_t));
    if (l2_tables * sizeof(uint64_t) > QCOW_MAX_L1_SIZE) {
        error_setg(&local_err, "The image size is too large "
                               "(try using a larger cluster size)");
        goto err;
    }

    /
    if (in_bs) {
        int64_t ssize = bdrv_getlength(in_bs);
        if (ssize < 0) {
            error_setg_errno(&local_err, -ssize,
                             "Unable to get image virtual_size");
            goto err;
        }

        virtual_size = align_offset(ssize, cluster_size);

        if (has_backing_file) {
            /
            required = virtual_size;
        } else {
            int cluster_sectors = cluster_size / BDRV_SECTOR_SIZE;
            int64_t sector_num;
            int pnum = 0;

            for (sector_num = 0;
                 sector_num < ssize / BDRV_SECTOR_SIZE;
                 sector_num += pnum) {
                int nb_sectors = MAX(ssize / BDRV_SECTOR_SIZE - sector_num,
                                     INT_MAX);
                BlockDriverState *file;
                int64_t ret;

                ret = bdrv_get_block_status_above(in_bs, NULL,
                                                  sector_num, nb_sectors,
                                                  &pnum, &file);
                if (ret < 0) {
                    error_setg_errno(&local_err, -ret,
                                     "Unable to get block status");
                    goto err;
                }

                if (ret & BDRV_BLOCK_ZERO) {
                    /
                } else if ((ret & (BDRV_BLOCK_DATA | BDRV_BLOCK_ALLOCATED)) ==
                           (BDRV_BLOCK_DATA | BDRV_BLOCK_ALLOCATED)) {
                    /
                    pnum = ROUND_UP(sector_num + pnum, cluster_sectors) -
                           sector_num;

                    /
                    required += (sector_num % cluster_sectors + pnum) *
                                BDRV_SECTOR_SIZE;
                }
            }
        }
    }

    /
    if (prealloc == PREALLOC_MODE_FULL || prealloc == PREALLOC_MODE_FALLOC) {
        required = virtual_size;
    }

    info = g_new(BlockMeasureInfo, 1);
    info->fully_allocated =
        qcow2_calc_prealloc_size(virtual_size, cluster_size,
                                 ctz32(refcount_bits));

    /
    info->required = info->fully_allocated - virtual_size + required;
    return info;

err:
    error_propagate(errp, local_err);
    return NULL;
}
