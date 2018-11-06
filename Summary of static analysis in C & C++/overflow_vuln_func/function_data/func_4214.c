static void vp8_decode_flush(AVCodecContext *avctx)
{
    vp8_decode_flush_impl(avctx, 0, 0);
}
