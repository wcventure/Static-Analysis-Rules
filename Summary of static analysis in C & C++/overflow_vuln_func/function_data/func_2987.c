void qdist_init(struct qdist *dist)
{
    dist->entries = g_malloc(sizeof(*dist->entries));
    dist->size = 1;
    dist->n = 0;
}
