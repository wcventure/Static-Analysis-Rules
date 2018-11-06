static int64_t alloc_clusters_noref(BlockDriverState *bs, uint64_t size)
{
    BDRVQcowState *s = bs->opaque;
    uint64_t i, nb_clusters;
    int refcount;

    nb_clusters = size_to_clusters(s, size);
retry:
    for(i = 0; i < nb_clusters; i++) {
        uint64_t next_cluster_index = s->free_cluster_index++;
        refcount = get_refcount(bs, next_cluster_index);

        if (refcount < 0) {
            return refcount;
        } else if (refcount != 0) {
            goto retry;
        }
    }

    /
    if (s->free_cluster_index - 1 > (INT64_MAX >> s->cluster_bits)) {
        return -EFBIG;
    }

#ifdef DEBUG_ALLOC2
    fprintf(stderr, "alloc_clusters: size=%" PRId64 " -> %" PRId64 "\n",
            size,
            (s->free_cluster_index - nb_clusters) << s->cluster_bits);
#endif
    return (s->free_cluster_index - nb_clusters) << s->cluster_bits;
}
