static void cloop_close(BlockDriverState *bs)
{
    BDRVCloopState *s = bs->opaque;
    if (s->n_blocks > 0) {
        g_free(s->offsets);
    }
    g_free(s->compressed_block);
    g_free(s->uncompressed_block);
    inflateEnd(&s->zstream);
}
