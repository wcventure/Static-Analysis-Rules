int qcow2_get_cluster_offset(BlockDriverState *bs, uint64_t offset,
                             unsigned int *bytes, uint64_t *cluster_offset)
{
    BDRVQcow2State *s = bs->opaque;
    unsigned int l2_index;
    uint64_t l1_index, l2_offset, *l2_table;
    int l1_bits, c;
    unsigned int offset_in_cluster, nb_clusters;
    uint64_t bytes_available, bytes_needed;
    int ret;

    offset_in_cluster = offset_into_cluster(s, offset);
    bytes_needed = (uint64_t) *bytes + offset_in_cluster;

    l1_bits = s->l2_bits + s->cluster_bits;

    /
    bytes_available = (1ULL << l1_bits) - (offset & ((1ULL << l1_bits) - 1))
                    + offset_in_cluster;

    if (bytes_needed > bytes_available) {
        bytes_needed = bytes_available;
    }
    assert(bytes_needed <= INT_MAX);

    *cluster_offset = 0;

    /

    l1_index = offset >> l1_bits;
    if (l1_index >= s->l1_size) {
        ret = QCOW2_CLUSTER_UNALLOCATED;
        goto out;
    }

    l2_offset = s->l1_table[l1_index] & L1E_OFFSET_MASK;
    if (!l2_offset) {
        ret = QCOW2_CLUSTER_UNALLOCATED;
        goto out;
    }

    if (offset_into_cluster(s, l2_offset)) {
        qcow2_signal_corruption(bs, true, -1, -1, "L2 table offset %#" PRIx64
                                " unaligned (L1 index: %#" PRIx64 ")",
                                l2_offset, l1_index);
        return -EIO;
    }

    /

    ret = l2_load(bs, l2_offset, &l2_table);
    if (ret < 0) {
        return ret;
    }

    /

    l2_index = (offset >> s->cluster_bits) & (s->l2_size - 1);
    *cluster_offset = be64_to_cpu(l2_table[l2_index]);

    /
    nb_clusters = size_to_clusters(s, bytes_needed);

    ret = qcow2_get_cluster_type(*cluster_offset);
    switch (ret) {
    case QCOW2_CLUSTER_COMPRESSED:
        /
        c = 1;
        *cluster_offset &= L2E_COMPRESSED_OFFSET_SIZE_MASK;
        break;
    case QCOW2_CLUSTER_ZERO:
        if (s->qcow_version < 3) {
            qcow2_signal_corruption(bs, true, -1, -1, "Zero cluster entry found"
                                    " in pre-v3 image (L2 offset: %#" PRIx64
                                    ", L2 index: %#x)", l2_offset, l2_index);
            ret = -EIO;
            goto fail;
        }
        c = count_contiguous_clusters_by_type(nb_clusters, &l2_table[l2_index],
                                              QCOW2_CLUSTER_ZERO);
        *cluster_offset = 0;
        break;
    case QCOW2_CLUSTER_UNALLOCATED:
        /
        c = count_contiguous_clusters_by_type(nb_clusters, &l2_table[l2_index],
                                              QCOW2_CLUSTER_UNALLOCATED);
        *cluster_offset = 0;
        break;
    case QCOW2_CLUSTER_NORMAL:
        /
        c = count_contiguous_clusters(nb_clusters, s->cluster_size,
                &l2_table[l2_index], QCOW_OFLAG_ZERO);
        *cluster_offset &= L2E_OFFSET_MASK;
        if (offset_into_cluster(s, *cluster_offset)) {
            qcow2_signal_corruption(bs, true, -1, -1, "Data cluster offset %#"
                                    PRIx64 " unaligned (L2 offset: %#" PRIx64
                                    ", L2 index: %#x)", *cluster_offset,
                                    l2_offset, l2_index);
            ret = -EIO;
            goto fail;
        }
        break;
    default:
        abort();
    }

    qcow2_cache_put(bs, s->l2_table_cache, (void**) &l2_table);

    bytes_available = (c * s->cluster_size);

out:
    if (bytes_available > bytes_needed) {
        bytes_available = bytes_needed;
    }

    *bytes = bytes_available - offset_in_cluster;

    return ret;

fail:
    qcow2_cache_put(bs, s->l2_table_cache, (void **)&l2_table);
    return ret;
}
