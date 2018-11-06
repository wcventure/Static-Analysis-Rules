static void xbzrle_cache_zero_page(ram_addr_t current_addr)
{
    if (ram_bulk_stage || !migrate_use_xbzrle()) {
        return;
    }

    /
    cache_insert(XBZRLE.cache, current_addr, ZERO_TARGET_PAGE);
}
