static void vmdk_free_last_extent(BlockDriverState *bs)
{
    BDRVVmdkState *s = bs->opaque;

    if (s->num_extents == 0) {
        return;
    }
    s->num_extents--;
    s->extents = g_realloc(s->extents, s->num_extents * sizeof(VmdkExtent));
}
