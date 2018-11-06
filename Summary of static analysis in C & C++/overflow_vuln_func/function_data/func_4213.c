static void vp8_decode_flush_impl(AVCodecContext *avctx, int force, int is_close)
{
    VP8Context *s = avctx->priv_data;
    int i;

    if (!avctx->is_copy || force) {
        for (i = 0; i < 5; i++)
            if (s->frames[i].data[0])
                vp8_release_frame(s, &s->frames[i], is_close);
    }
    memset(s->framep, 0, sizeof(s->framep));

    free_buffers(s);
    s->maps_are_invalid = 1;
}
