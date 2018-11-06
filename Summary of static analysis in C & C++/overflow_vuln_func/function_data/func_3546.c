static int y41p_decode_frame(AVCodecContext *avctx, void *data,
                             int *got_frame, AVPacket *avpkt)
{
    AVFrame *pic = data;
    uint8_t *src = avpkt->data;
    uint8_t *y, *u, *v;
    int i, j, ret;

    if (avpkt->size < 3LL * avctx->height * avctx->width / 2) {
        av_log(avctx, AV_LOG_ERROR, "Insufficient input data.\n");
        return AVERROR(EINVAL);
    }

    if ((ret = ff_get_buffer(avctx, pic, 0)) < 0)
        return ret;

    pic->key_frame = 1;
    pic->pict_type = AV_PICTURE_TYPE_I;

    for (i = avctx->height - 1; i >= 0 ; i--) {
        y = &pic->data[0][i * pic->linesize[0]];
        u = &pic->data[1][i * pic->linesize[1]];
        v = &pic->data[2][i * pic->linesize[2]];
        for (j = 0; j < avctx->width; j += 8) {
            *(u++) = *src++;
            *(y++) = *src++;
            *(v++) = *src++;
            *(y++) = *src++;

            *(u++) = *src++;
            *(y++) = *src++;
            *(v++) = *src++;
            *(y++) = *src++;

            *(y++) = *src++;
            *(y++) = *src++;
            *(y++) = *src++;
            *(y++) = *src++;
        }
    }

    *got_frame = 1;

    return avpkt->size;
}
