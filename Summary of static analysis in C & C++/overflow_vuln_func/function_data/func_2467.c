static int qcow2_open(BlockDriverState *bs, QDict *options, int flags)
{
    BDRVQcowState *s = bs->opaque;
    int len, i, ret = 0;
    QCowHeader header;
    QemuOpts *opts;
    Error *local_err = NULL;
    uint64_t ext_end;

    ret = bdrv_pread(bs->file, 0, &header, sizeof(header));
    if (ret < 0) {
        goto fail;
    }
    be32_to_cpus(&header.magic);
    be32_to_cpus(&header.version);
    be64_to_cpus(&header.backing_file_offset);
    be32_to_cpus(&header.backing_file_size);
    be64_to_cpus(&header.size);
    be32_to_cpus(&header.cluster_bits);
    be32_to_cpus(&header.crypt_method);
    be64_to_cpus(&header.l1_table_offset);
    be32_to_cpus(&header.l1_size);
    be64_to_cpus(&header.refcount_table_offset);
    be32_to_cpus(&header.refcount_table_clusters);
    be64_to_cpus(&header.snapshots_offset);
    be32_to_cpus(&header.nb_snapshots);

    if (header.magic != QCOW_MAGIC) {
        ret = -EMEDIUMTYPE;
        goto fail;
    }
    if (header.version < 2 || header.version > 3) {
        report_unsupported(bs, "QCOW version %d", header.version);
        ret = -ENOTSUP;
        goto fail;
    }

    s->qcow_version = header.version;

    /
    if (header.version == 2) {
        header.incompatible_features    = 0;
        header.compatible_features      = 0;
        header.autoclear_features       = 0;
        header.refcount_order           = 4;
        header.header_length            = 72;
    } else {
        be64_to_cpus(&header.incompatible_features);
        be64_to_cpus(&header.compatible_features);
        be64_to_cpus(&header.autoclear_features);
        be32_to_cpus(&header.refcount_order);
        be32_to_cpus(&header.header_length);
    }

    if (header.header_length > sizeof(header)) {
        s->unknown_header_fields_size = header.header_length - sizeof(header);
        s->unknown_header_fields = g_malloc(s->unknown_header_fields_size);
        ret = bdrv_pread(bs->file, sizeof(header), s->unknown_header_fields,
                         s->unknown_header_fields_size);
        if (ret < 0) {
            goto fail;
        }
    }

    if (header.backing_file_offset) {
        ext_end = header.backing_file_offset;
    } else {
        ext_end = 1 << header.cluster_bits;
    }

    /
    s->incompatible_features    = header.incompatible_features;
    s->compatible_features      = header.compatible_features;
    s->autoclear_features       = header.autoclear_features;

    if (s->incompatible_features & ~QCOW2_INCOMPAT_MASK) {
        void *feature_table = NULL;
        qcow2_read_extensions(bs, header.header_length, ext_end,
                              &feature_table);
        report_unsupported_feature(bs, feature_table,
                                   s->incompatible_features &
                                   ~QCOW2_INCOMPAT_MASK);
        ret = -ENOTSUP;
        goto fail;
    }

    /
    if (header.refcount_order != 4) {
        report_unsupported(bs, "%d bit reference counts",
                           1 << header.refcount_order);
        ret = -ENOTSUP;
        goto fail;
    }

    if (header.cluster_bits < MIN_CLUSTER_BITS ||
        header.cluster_bits > MAX_CLUSTER_BITS) {
        ret = -EINVAL;
        goto fail;
    }
    if (header.crypt_method > QCOW_CRYPT_AES) {
        ret = -EINVAL;
        goto fail;
    }
    s->crypt_method_header = header.crypt_method;
    if (s->crypt_method_header) {
        bs->encrypted = 1;
    }
    s->cluster_bits = header.cluster_bits;
    s->cluster_size = 1 << s->cluster_bits;
    s->cluster_sectors = 1 << (s->cluster_bits - 9);
    s->l2_bits = s->cluster_bits - 3; /
    s->l2_size = 1 << s->l2_bits;
    bs->total_sectors = header.size / 512;
    s->csize_shift = (62 - (s->cluster_bits - 8));
    s->csize_mask = (1 << (s->cluster_bits - 8)) - 1;
    s->cluster_offset_mask = (1LL << s->csize_shift) - 1;
    s->refcount_table_offset = header.refcount_table_offset;
    s->refcount_table_size =
        header.refcount_table_clusters << (s->cluster_bits - 3);

    s->snapshots_offset = header.snapshots_offset;
    s->nb_snapshots = header.nb_snapshots;

    /
    s->l1_size = header.l1_size;
    s->l1_vm_state_index = size_to_l1(s, header.size);
    /
    if (s->l1_size < s->l1_vm_state_index) {
        ret = -EINVAL;
        goto fail;
    }
    s->l1_table_offset = header.l1_table_offset;
    if (s->l1_size > 0) {
        s->l1_table = g_malloc0(
            align_offset(s->l1_size * sizeof(uint64_t), 512));
        ret = bdrv_pread(bs->file, s->l1_table_offset, s->l1_table,
                         s->l1_size * sizeof(uint64_t));
        if (ret < 0) {
            goto fail;
        }
        for(i = 0;i < s->l1_size; i++) {
            be64_to_cpus(&s->l1_table[i]);
        }
    }

    /
    s->l2_table_cache = qcow2_cache_create(bs, L2_CACHE_SIZE);
    s->refcount_block_cache = qcow2_cache_create(bs, REFCOUNT_CACHE_SIZE);

    s->cluster_cache = g_malloc(s->cluster_size);
    /
    s->cluster_data = qemu_blockalign(bs, QCOW_MAX_CRYPT_CLUSTERS * s->cluster_size
                                  + 512);
    s->cluster_cache_offset = -1;
    s->flags = flags;

    ret = qcow2_refcount_init(bs);
    if (ret != 0) {
        goto fail;
    }

    QLIST_INIT(&s->cluster_allocs);

    /
    if (qcow2_read_extensions(bs, header.header_length, ext_end, NULL)) {
        ret = -EINVAL;
        goto fail;
    }

    /
    if (header.backing_file_offset != 0) {
        len = header.backing_file_size;
        if (len > 1023) {
            len = 1023;
        }
        ret = bdrv_pread(bs->file, header.backing_file_offset,
                         bs->backing_file, len);
        if (ret < 0) {
            goto fail;
        }
        bs->backing_file[len] = '\0';
    }

    ret = qcow2_read_snapshots(bs);
    if (ret < 0) {
        goto fail;
    }

    /
    if (!bs->read_only && s->autoclear_features != 0) {
        s->autoclear_features = 0;
        ret = qcow2_update_header(bs);
        if (ret < 0) {
            goto fail;
        }
    }

    /
    qemu_co_mutex_init(&s->lock);

    /
    if (!(flags & BDRV_O_CHECK) && !bs->read_only &&
        (s->incompatible_features & QCOW2_INCOMPAT_DIRTY)) {
        BdrvCheckResult result = {0};

        ret = qcow2_check(bs, &result, BDRV_FIX_ERRORS);
        if (ret < 0) {
            goto fail;
        }
    }

    /
    opts = qemu_opts_create_nofail(&qcow2_runtime_opts);
    qemu_opts_absorb_qdict(opts, options, &local_err);
    if (error_is_set(&local_err)) {
        qerror_report_err(local_err);
        error_free(local_err);
        ret = -EINVAL;
        goto fail;
    }

    s->use_lazy_refcounts = qemu_opt_get_bool(opts, QCOW2_OPT_LAZY_REFCOUNTS,
        (s->compatible_features & QCOW2_COMPAT_LAZY_REFCOUNTS));

    qemu_opts_del(opts);

    if (s->use_lazy_refcounts && s->qcow_version < 3) {
        qerror_report(ERROR_CLASS_GENERIC_ERROR, "Lazy refcounts require "
            "a qcow2 image with at least qemu 1.1 compatibility level");
        ret = -EINVAL;
        goto fail;
    }

#ifdef DEBUG_ALLOC
    {
        BdrvCheckResult result = {0};
        qcow2_check_refcounts(bs, &result, 0);
    }
#endif
    return ret;

 fail:
    g_free(s->unknown_header_fields);
    cleanup_unknown_header_ext(bs);
    qcow2_free_snapshots(bs);
    qcow2_refcount_close(bs);
    g_free(s->l1_table);
    if (s->l2_table_cache) {
        qcow2_cache_destroy(bs, s->l2_table_cache);
    }
    g_free(s->cluster_cache);
    qemu_vfree(s->cluster_data);
    return ret;
}
