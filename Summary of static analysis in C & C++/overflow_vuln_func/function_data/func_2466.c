static int get_cluster_table(BlockDriverState *bs, uint64_t offset,
                             uint64_t **new_l2_table,
                             int *new_l2_index)
{
    BDRVQcowState *s = bs->opaque;
    unsigned int l1_index, l2_index;
    uint64_t l2_offset;
    uint64_t *l2_table = NULL;
    int ret;

    /

    l1_index = offset >> (s->l2_bits + s->cluster_bits);
    if (l1_index >= s->l1_size) {
        ret = qcow2_grow_l1_table(bs, l1_index + 1, false);
        if (ret < 0) {
            return ret;
        }
    }

    l2_offset = s->l1_table[l1_index] & L1E_OFFSET_MASK;

    /

    if (s->l1_table[l1_index] & QCOW_OFLAG_COPIED) {
        /
        ret = l2_load(bs, l2_offset, &l2_table);
        if (ret < 0) {
            return ret;
        }
    } else {
        /
        ret = l2_allocate(bs, l1_index, &l2_table);
        if (ret < 0) {
            return ret;
        }

        /
        if (l2_offset) {
            qcow2_free_clusters(bs, l2_offset, s->l2_size * sizeof(uint64_t));
        }
    }

    /

    l2_index = (offset >> s->cluster_bits) & (s->l2_size - 1);

    *new_l2_table = l2_table;
    *new_l2_index = l2_index;

    return 0;
}
