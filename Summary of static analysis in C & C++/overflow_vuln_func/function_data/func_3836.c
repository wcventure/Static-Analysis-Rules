static int cmp(const void *key, const void *node)
{
    return (*(const int64_t *) key) - ((const CacheEntry *) node)->logical_pos;
}
