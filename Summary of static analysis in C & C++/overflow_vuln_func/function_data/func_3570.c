static int dnxhd_decode_frame(AVCodecContext *avctx, void *data,
                              int *got_frame, AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    DNXHDContext *ctx = avctx->priv_data;
    ThreadFrame frame = { .f = data };
    AVFrame *picture = data;
    int first_field = 1;
    int ret;

    ff_dlog(avctx, "frame size %d\n", buf_size);

decode_coding_unit:
    if ((ret = dnxhd_decode_header(ctx, picture, buf, buf_size, first_field)) < 0)
        return ret;

    if ((avctx->width || avctx->height) &&
        (ctx->width != avctx->width || ctx->height != avctx->height)) {
        av_log(avctx, AV_LOG_WARNING, "frame size changed: %dx%d -> %dx%d\n",
               avctx->width, avctx->height, ctx->width, ctx->height);
        first_field = 1;
    }
    if (avctx->pix_fmt != AV_PIX_FMT_NONE && avctx->pix_fmt != ctx->pix_fmt) {
        av_log(avctx, AV_LOG_WARNING, "pix_fmt changed: %s -> %s\n",
               av_get_pix_fmt_name(avctx->pix_fmt), av_get_pix_fmt_name(ctx->pix_fmt));
        first_field = 1;
    }

    avctx->pix_fmt = ctx->pix_fmt;
    ret = ff_set_dimensions(avctx, ctx->width, ctx->height);
    if (ret < 0)
        return ret;

    if (first_field) {
        if ((ret = ff_thread_get_buffer(avctx, &frame, 0)) < 0)
            return ret;
        picture->pict_type = AV_PICTURE_TYPE_I;
        picture->key_frame = 1;
    }

    ctx->buf_size = buf_size - 0x280;
    ctx->buf = buf + 0x280;
    avctx->execute2(avctx, dnxhd_decode_row, picture, NULL, ctx->mb_height);

    if (first_field && picture->interlaced_frame) {
        buf      += ctx->cid_table->coding_unit_size;
        buf_size -= ctx->cid_table->coding_unit_size;
        first_field = 0;
        goto decode_coding_unit;
    }

    *got_frame = 1;
    return avpkt->size;
}
