static int mirror_cow_align(MirrorBlockJob *s, int64_t *offset,
                            uint64_t *bytes)
{
    bool need_cow;
    int ret = 0;
    int64_t align_offset = *offset;
    unsigned int align_bytes = *bytes;
    int max_bytes = s->granularity * s->max_iov;

    assert(*bytes < INT_MAX);
    need_cow = !test_bit(*offset / s->granularity, s->cow_bitmap);
    need_cow |= !test_bit((*offset + *bytes - 1) / s->granularity,
                          s->cow_bitmap);
    if (need_cow) {
        bdrv_round_to_clusters(blk_bs(s->target), *offset, *bytes,
                               &align_offset, &align_bytes);
    }

    if (align_bytes > max_bytes) {
        align_bytes = max_bytes;
        if (need_cow) {
            align_bytes = QEMU_ALIGN_DOWN(align_bytes, s->target_cluster_size);
        }
    }
    /
    align_bytes = mirror_clip_bytes(s, align_offset, align_bytes);

    ret = align_offset + align_bytes - (*offset + *bytes);
    *offset = align_offset;
    *bytes = align_bytes;
    assert(ret >= 0);
    return ret;
}
