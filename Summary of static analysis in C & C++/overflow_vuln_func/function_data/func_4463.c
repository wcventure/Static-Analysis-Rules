static int cache_read(URLContext *h, unsigned char *buf, int size)
{
    Context *c= h->priv_data;
    CacheEntry *entry, *next[2] = {NULL, NULL};
    int r;

    entry = av_tree_find(c->root, &c->logical_pos, cmp, (void**)next);

    if (!entry)
        entry = next[0];

    if (entry) {
        int64_t in_block_pos = c->logical_pos - entry->logical_pos;
        av_assert0(entry->logical_pos <= c->logical_pos);
        if (in_block_pos < entry->size) {
            int64_t physical_target = entry->physical_pos + in_block_pos;

            if (c->cache_pos != physical_target) {
                r = lseek(c->fd, physical_target, SEEK_SET);
            } else
                r = c->cache_pos;

            if (r >= 0) {
                c->cache_pos = r;
                r = read(c->fd, buf, FFMIN(size, entry->size - in_block_pos));
            }

            if (r > 0) {
                c->cache_pos += r;
                c->logical_pos += r;
                c->cache_hit ++;
                return r;
            }
        }
    }

    // Cache miss or some kind of fault with the cache

    if (c->logical_pos != c->inner_pos) {
        r = ffurl_seek(c->inner, c->logical_pos, SEEK_SET);
        if (r<0) {
            av_log(h, AV_LOG_ERROR, "Failed to perform internal seek\n");
            return r;
        }
        c->inner_pos = r;
    }

    r = ffurl_read(c->inner, buf, size);
    if (r == 0 && size>0) {
        c->is_true_eof = 1;
        av_assert0(c->end >= c->logical_pos);
    }
    if (r<=0)
        return r;
    c->inner_pos += r;

    c->cache_miss ++;

    add_entry(h, buf, r);
    c->logical_pos += r;
    c->end = FFMAX(c->end, c->logical_pos);

    return r;
}
