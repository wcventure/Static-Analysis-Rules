static int y216_decode_frame(AVCodecContext *avctx, void *data,
                             int *got_frame, AVPacket *avpkt)
{
    AVFrame *pic = data;
    const uint16_t *src = (uint16_t *)avpkt->data;
    uint16_t *y, *u, *v, aligned_width = FFALIGN(avctx->width, 4);
    int i, j, ret;

    if (avpkt->size < 4 * avctx->height * aligned_width) {
        av_log(avctx, AV_LOG_ERROR, "Insufficient input data.\n");
        return AVERROR(EINVAL);
    }

    if ((ret = ff_get_buffer(avctx, pic, 0)) < 0)
        return ret;

    pic->key_frame = 1;
    pic->pict_type = AV_PICTURE_TYPE_I;

    y = (uint16_t *)pic->data[0];
    u = (uint16_t *)pic->data[1];
    v = (uint16_t *)pic->data[2];

    for (i = 0; i < avctx->height; i++) {
        for (j = 0; j < avctx->width >> 1; j++) {
            u[    j    ] = src[4 * j    ] << 2 | src[4 * j    ] >> 14;
            y[2 * j    ] = src[4 * j + 1] << 2 | src[4 * j + 1] >> 14;
            v[    j    ] = src[4 * j + 2] << 2 | src[4 * j + 2] >> 14;
            y[2 * j + 1] = src[4 * j + 3] << 2 | src[4 * j + 3] >> 14;
        }

        y += pic->linesize[0] >> 1;
        u += pic->linesize[1] >> 1;
        v += pic->linesize[2] >> 1;
        src += aligned_width << 1;
    }

    *got_frame = 1;

    return avpkt->size;
}
