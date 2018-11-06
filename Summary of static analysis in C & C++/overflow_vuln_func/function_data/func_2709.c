int cache_insert(PageCache *cache, uint64_t addr, const uint8_t *pdata)
{

    CacheItem *it = NULL;

    g_assert(cache);
    g_assert(cache->page_cache);

    /
    it = cache_get_by_addr(cache, addr);

    /
    if (!it->it_data) {
        it->it_data = g_try_malloc(cache->page_size);
        if (!it->it_data) {
            DPRINTF("Error allocating page\n");
            return -1;
        }
        cache->num_items++;
    }

    memcpy(it->it_data, pdata, cache->page_size);

    it->it_age = ++cache->max_item_age;
    it->it_addr = addr;

    return 0;
}
