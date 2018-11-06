static int dxtory_decode_v1_444(AVCodecContext *avctx, AVFrame *pic,
                                const uint8_t *src, int src_size)
{
    int h, w;
    uint8_t *Y, *U, *V;
    int ret;

    if (src_size < avctx->width * avctx->height * 3) {
        av_log(avctx, AV_LOG_ERROR, "packet too small\n");
        return AVERROR_INVALIDDATA;
    }

    avctx->pix_fmt = AV_PIX_FMT_YUV444P;
    if ((ret = ff_get_buffer(avctx, pic, 0)) < 0)
        return ret;

    Y = pic->data[0];
    U = pic->data[1];
    V = pic->data[2];
    for (h = 0; h < avctx->height; h++) {
        for (w = 0; w < avctx->width; w++) {
            Y[w] = *src++;
            U[w] = *src++ ^ 0x80;
            V[w] = *src++ ^ 0x80;
        }
        Y += pic->linesize[0];
        U += pic->linesize[1];
        V += pic->linesize[2];
    }

    return 0;
}
