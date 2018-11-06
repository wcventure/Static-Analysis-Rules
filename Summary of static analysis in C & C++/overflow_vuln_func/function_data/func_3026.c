int64_t qcow2_alloc_clusters(BlockDriverState *bs, int64_t size)
{
    int64_t offset;
    int ret;

    BLKDBG_EVENT(bs->file, BLKDBG_CLUSTER_ALLOC);
    do {
        offset = alloc_clusters_noref(bs, size);
        if (offset < 0) {
            return offset;
        }

        ret = update_refcount(bs, offset, size, 1, QCOW2_DISCARD_NEVER);
    } while (ret == -EAGAIN);

    if (ret < 0) {
        return ret;
    }

    return offset;
}
