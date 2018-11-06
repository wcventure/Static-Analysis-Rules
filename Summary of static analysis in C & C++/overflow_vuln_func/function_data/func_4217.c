static av_cold int vp8_decode_free(AVCodecContext *avctx)
{
    vp8_decode_flush_impl(avctx, 0, 1);
    release_queued_segmaps(avctx->priv_data, 1);
    return 0;
}
