static int dxtory_decode_v1_410(AVCodecContext *avctx, AVFrame *pic,
                                const uint8_t *src, int src_size)
{
    int h, w;
    uint8_t *Y1, *Y2, *Y3, *Y4, *U, *V;
    int ret;

    if (src_size < avctx->width * avctx->height * 9L / 8) {
        av_log(avctx, AV_LOG_ERROR, "packet too small\n");
        return AVERROR_INVALIDDATA;
    }

    avctx->pix_fmt = AV_PIX_FMT_YUV410P;
    if ((ret = ff_get_buffer(avctx, pic, 0)) < 0)
        return ret;

    Y1 = pic->data[0];
    Y2 = pic->data[0] + pic->linesize[0];
    Y3 = pic->data[0] + pic->linesize[0] * 2;
    Y4 = pic->data[0] + pic->linesize[0] * 3;
    U  = pic->data[1];
    V  = pic->data[2];
    for (h = 0; h < avctx->height; h += 4) {
        for (w = 0; w < avctx->width; w += 4) {
            AV_COPY32(Y1 + w, src);
            AV_COPY32(Y2 + w, src + 4);
            AV_COPY32(Y3 + w, src + 8);
            AV_COPY32(Y4 + w, src + 12);
            U[w >> 2] = src[16] + 0x80;
            V[w >> 2] = src[17] + 0x80;
            src += 18;
        }
        Y1 += pic->linesize[0] << 2;
        Y2 += pic->linesize[0] << 2;
        Y3 += pic->linesize[0] << 2;
        Y4 += pic->linesize[0] << 2;
        U  += pic->linesize[1];
        V  += pic->linesize[2];
    }

    return 0;
}
