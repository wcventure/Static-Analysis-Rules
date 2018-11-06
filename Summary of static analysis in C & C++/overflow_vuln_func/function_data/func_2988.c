void qdist_add(struct qdist *dist, double x, long count)
{
    struct qdist_entry *entry = NULL;

    if (dist->n) {
        struct qdist_entry e;

        e.x = x;
        entry = bsearch(&e, dist->entries, dist->n, sizeof(e), qdist_cmp);
    }

    if (entry) {
        entry->count += count;
        return;
    }

    if (unlikely(dist->n == dist->size)) {
        dist->size *= 2;
        dist->entries = g_realloc(dist->entries,
                                  sizeof(*dist->entries) * (dist->size));
    }
    dist->n++;
    entry = &dist->entries[dist->n - 1];
    entry->x = x;
    entry->count = count;
    qsort(dist->entries, dist->n, sizeof(*entry), qdist_cmp);
}
