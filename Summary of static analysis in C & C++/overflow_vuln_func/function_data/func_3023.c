int qcow2_alloc_cluster_offset(BlockDriverState *bs, uint64_t offset,
    int *num, uint64_t *host_offset, QCowL2Meta **m)
{
    BDRVQcowState *s = bs->opaque;
    uint64_t start, remaining;
    uint64_t cluster_offset;
    uint64_t cur_bytes;
    int ret;

    trace_qcow2_alloc_clusters_offset(qemu_coroutine_self(), offset, *num);

    assert((offset & ~BDRV_SECTOR_MASK) == 0);

again:
    start = offset;
    remaining = *num << BDRV_SECTOR_BITS;
    cluster_offset = 0;
    *host_offset = 0;
    cur_bytes = 0;
    *m = NULL;

    while (true) {

        if (!*host_offset) {
            *host_offset = start_of_cluster(s, cluster_offset);
        }

        assert(remaining >= cur_bytes);

        start           += cur_bytes;
        remaining       -= cur_bytes;
        cluster_offset  += cur_bytes;

        if (remaining == 0) {
            break;
        }

        cur_bytes = remaining;

        /
        ret = handle_dependencies(bs, start, &cur_bytes, m);
        if (ret == -EAGAIN) {
            /
            assert(*m == NULL);
            goto again;
        } else if (ret < 0) {
            return ret;
        } else if (cur_bytes == 0) {
            break;
        } else {
            /
        }

        /
        ret = handle_copied(bs, start, &cluster_offset, &cur_bytes, m);
        if (ret < 0) {
            return ret;
        } else if (ret) {
            continue;
        } else if (cur_bytes == 0) {
            break;
        }

        /
        ret = handle_alloc(bs, start, &cluster_offset, &cur_bytes, m);
        if (ret < 0) {
            return ret;
        } else if (ret) {
            continue;
        } else {
            assert(cur_bytes == 0);
            break;
        }
    }

    *num -= remaining >> BDRV_SECTOR_BITS;
    assert(*num > 0);
    assert(*host_offset != 0);

    return 0;
}
