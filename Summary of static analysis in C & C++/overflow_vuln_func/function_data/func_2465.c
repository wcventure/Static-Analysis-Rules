int qcow2_get_cluster_offset(BlockDriverState *bs, uint64_t offset,
    int *num, uint64_t *cluster_offset)
{
    BDRVQcowState *s = bs->opaque;
    unsigned int l1_index, l2_index;
    uint64_t l2_offset, *l2_table;
    int l1_bits, c;
    unsigned int index_in_cluster, nb_clusters;
    uint64_t nb_available, nb_needed;
    int ret;

    index_in_cluster = (offset >> 9) & (s->cluster_sectors - 1);
    nb_needed = *num + index_in_cluster;

    l1_bits = s->l2_bits + s->cluster_bits;

    /

    nb_available = (1ULL << l1_bits) - (offset & ((1ULL << l1_bits) - 1));

    /

    nb_available = (nb_available >> 9) + index_in_cluster;

    if (nb_needed > nb_available) {
        nb_needed = nb_available;
    }

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

    /

    ret = l2_load(bs, l2_offset, &l2_table);
    if (ret < 0) {
        return ret;
    }

    /

    l2_index = (offset >> s->cluster_bits) & (s->l2_size - 1);
    *cluster_offset = be64_to_cpu(l2_table[l2_index]);
    nb_clusters = size_to_clusters(s, nb_needed << 9);

    ret = qcow2_get_cluster_type(*cluster_offset);
    switch (ret) {
    case QCOW2_CLUSTER_COMPRESSED:
        /
        c = 1;
        *cluster_offset &= L2E_COMPRESSED_OFFSET_SIZE_MASK;
        break;
    case QCOW2_CLUSTER_ZERO:
        if (s->qcow_version < 3) {
            return -EIO;
        }
        c = count_contiguous_clusters(nb_clusters, s->cluster_size,
                &l2_table[l2_index], 0,
                QCOW_OFLAG_COMPRESSED | QCOW_OFLAG_ZERO);
        *cluster_offset = 0;
        break;
    case QCOW2_CLUSTER_UNALLOCATED:
        /
        c = count_contiguous_free_clusters(nb_clusters, &l2_table[l2_index]);
        *cluster_offset = 0;
        break;
    case QCOW2_CLUSTER_NORMAL:
        /
        c = count_contiguous_clusters(nb_clusters, s->cluster_size,
                &l2_table[l2_index], 0,
                QCOW_OFLAG_COMPRESSED | QCOW_OFLAG_ZERO);
        *cluster_offset &= L2E_OFFSET_MASK;
        break;
    default:
        abort();
    }

    qcow2_cache_put(bs, s->l2_table_cache, (void**) &l2_table);

    nb_available = (c * s->cluster_sectors);

out:
    if (nb_available > nb_needed)
        nb_available = nb_needed;

    *num = nb_available - index_in_cluster;

    return ret;
}
