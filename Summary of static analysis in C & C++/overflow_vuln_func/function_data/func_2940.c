Qcow2Cache *qcow2_cache_create(BlockDriverState *bs, int num_tables)
{
    BDRVQcowState *s = bs->opaque;
    Qcow2Cache *c;
    int i;

    c = g_malloc0(sizeof(*c));
    c->size = num_tables;
    c->entries = g_malloc0(sizeof(*c->entries) * num_tables);

    for (i = 0; i < c->size; i++) {
        c->entries[i].table = qemu_try_blockalign(bs->file, s->cluster_size);
        if (c->entries[i].table == NULL) {
            goto fail;
        }
    }

    return c;

fail:
    for (i = 0; i < c->size; i++) {
        qemu_vfree(c->entries[i].table);
    }
    g_free(c->entries);
    g_free(c);
    return NULL;
}
