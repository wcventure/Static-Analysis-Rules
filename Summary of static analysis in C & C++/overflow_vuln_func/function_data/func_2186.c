static uint64_t coroutine_fn mirror_iteration(MirrorBlockJob *s)
{
    BlockDriverState *source = s->source;
    int64_t offset, first_chunk;
    uint64_t delay_ns = 0;
    /
    int nb_chunks = 1;
    int sectors_per_chunk = s->granularity >> BDRV_SECTOR_BITS;
    bool write_zeroes_ok = bdrv_can_write_zeroes_with_unmap(blk_bs(s->target));
    int max_io_bytes = MAX(s->buf_size / MAX_IN_FLIGHT, MAX_IO_BYTES);

    bdrv_dirty_bitmap_lock(s->dirty_bitmap);
    offset = bdrv_dirty_iter_next(s->dbi);
    if (offset < 0) {
        bdrv_set_dirty_iter(s->dbi, 0);
        offset = bdrv_dirty_iter_next(s->dbi);
        trace_mirror_restart_iter(s, bdrv_get_dirty_count(s->dirty_bitmap));
        assert(offset >= 0);
    }
    bdrv_dirty_bitmap_unlock(s->dirty_bitmap);

    first_chunk = offset / s->granularity;
    while (test_bit(first_chunk, s->in_flight_bitmap)) {
        trace_mirror_yield_in_flight(s, offset, s->in_flight);
        mirror_wait_for_io(s);
    }

    block_job_pause_point(&s->common);

    /
    bdrv_dirty_bitmap_lock(s->dirty_bitmap);
    while (nb_chunks * s->granularity < s->buf_size) {
        int64_t next_dirty;
        int64_t next_offset = offset + nb_chunks * s->granularity;
        int64_t next_chunk = next_offset / s->granularity;
        if (next_offset >= s->bdev_length ||
            !bdrv_get_dirty_locked(source, s->dirty_bitmap, next_offset)) {
            break;
        }
        if (test_bit(next_chunk, s->in_flight_bitmap)) {
            break;
        }

        next_dirty = bdrv_dirty_iter_next(s->dbi);
        if (next_dirty > next_offset || next_dirty < 0) {
            /
            bdrv_set_dirty_iter(s->dbi, next_offset);
            next_dirty = bdrv_dirty_iter_next(s->dbi);
        }
        assert(next_dirty == next_offset);
        nb_chunks++;
    }

    /
    bdrv_reset_dirty_bitmap_locked(s->dirty_bitmap, offset,
                                   nb_chunks * s->granularity);
    bdrv_dirty_bitmap_unlock(s->dirty_bitmap);

    bitmap_set(s->in_flight_bitmap, offset / s->granularity, nb_chunks);
    while (nb_chunks > 0 && offset < s->bdev_length) {
        int64_t ret;
        int io_sectors;
        unsigned int io_bytes;
        int64_t io_bytes_acct;
        enum MirrorMethod {
            MIRROR_METHOD_COPY,
            MIRROR_METHOD_ZERO,
            MIRROR_METHOD_DISCARD
        } mirror_method = MIRROR_METHOD_COPY;

        assert(!(offset % s->granularity));
        ret = bdrv_get_block_status_above(source, NULL,
                                          offset >> BDRV_SECTOR_BITS,
                                          nb_chunks * sectors_per_chunk,
                                          &io_sectors, NULL);
        io_bytes = io_sectors * BDRV_SECTOR_SIZE;
        if (ret < 0) {
            io_bytes = MIN(nb_chunks * s->granularity, max_io_bytes);
        } else if (ret & BDRV_BLOCK_DATA) {
            io_bytes = MIN(io_bytes, max_io_bytes);
        }

        io_bytes -= io_bytes % s->granularity;
        if (io_bytes < s->granularity) {
            io_bytes = s->granularity;
        } else if (ret >= 0 && !(ret & BDRV_BLOCK_DATA)) {
            int64_t target_offset;
            unsigned int target_bytes;
            bdrv_round_to_clusters(blk_bs(s->target), offset, io_bytes,
                                   &target_offset, &target_bytes);
            if (target_offset == offset &&
                target_bytes == io_bytes) {
                mirror_method = ret & BDRV_BLOCK_ZERO ?
                                    MIRROR_METHOD_ZERO :
                                    MIRROR_METHOD_DISCARD;
            }
        }

        while (s->in_flight >= MAX_IN_FLIGHT) {
            trace_mirror_yield_in_flight(s, offset, s->in_flight);
            mirror_wait_for_io(s);
        }

        if (s->ret < 0) {
            return 0;
        }

        io_bytes = mirror_clip_bytes(s, offset, io_bytes);
        switch (mirror_method) {
        case MIRROR_METHOD_COPY:
            io_bytes = io_bytes_acct = mirror_do_read(s, offset, io_bytes);
            break;
        case MIRROR_METHOD_ZERO:
        case MIRROR_METHOD_DISCARD:
            mirror_do_zero_or_discard(s, offset, io_bytes,
                                      mirror_method == MIRROR_METHOD_DISCARD);
            if (write_zeroes_ok) {
                io_bytes_acct = 0;
            } else {
                io_bytes_acct = io_bytes;
            }
            break;
        default:
            abort();
        }
        assert(io_bytes);
        offset += io_bytes;
        nb_chunks -= DIV_ROUND_UP(io_bytes, s->granularity);
        if (s->common.speed) {
            delay_ns = ratelimit_calculate_delay(&s->limit, io_bytes_acct);
        }
    }
    return delay_ns;
}
