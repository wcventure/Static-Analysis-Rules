bool cache_is_cached(const PageCache *cache, uint64_t addr)
{
    size_t pos;

    g_assert(cache);
    g_assert(cache->page_cache);

    pos = cache_get_cache_pos(cache, addr);

    return (cache->page_cache[pos].it_addr == addr);
}
