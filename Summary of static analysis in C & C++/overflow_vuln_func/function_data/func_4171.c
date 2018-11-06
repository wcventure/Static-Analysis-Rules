static int dxtory_decode_v1_rgb(AVCodecContext *avctx, AVFrame *pic,
                                const uint8_t *src, int src_size,
                                int id, int bpp)
{
    int h;
    uint8_t *dst;
    int ret;

    if (src_size < avctx->width * avctx->height * bpp) {
        av_log(avctx, AV_LOG_ERROR, "packet too small\n");
        return AVERROR_INVALIDDATA;
    }

    avctx->pix_fmt = id;
    if ((ret = ff_get_buffer(avctx, pic, 0)) < 0)
        return ret;

    dst = pic->data[0];
    for (h = 0; h < avctx->height; h++) {
        memcpy(dst, src, avctx->width * bpp);
        src += avctx->width * bpp;
        dst += pic->linesize[0];
    }

    return 0;
}
