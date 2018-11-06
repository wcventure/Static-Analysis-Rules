static int zero12v_decode_frame(AVCodecContext *avctx, void *data,
                                int *got_frame, AVPacket *avpkt)
{
    int line = 0, ret;
    const int width = avctx->width;
    AVFrame *pic = data;
    uint16_t *y, *u, *v;
    const uint8_t *line_end, *src = avpkt->data;
    int stride = avctx->width * 8 / 3;

    if (width <= 1 || avctx->height <= 0) {
        av_log(avctx, AV_LOG_ERROR, "Dimensions %dx%d not supported.\n", width, avctx->height);
        return AVERROR_INVALIDDATA;
    }

    if (   avctx->codec_tag == MKTAG('0', '1', '2', 'v')
        && avpkt->size % avctx->height == 0
        && avpkt->size / avctx->height * 3 >= width * 8)
        stride = avpkt->size / avctx->height;

    if (avpkt->size < avctx->height * stride) {
        av_log(avctx, AV_LOG_ERROR, "Packet too small: %d instead of %d\n",
               avpkt->size, avctx->height * stride);
        return AVERROR_INVALIDDATA;
    }

    if ((ret = ff_get_buffer(avctx, pic, 0)) < 0)
        return ret;

    pic->pict_type = AV_PICTURE_TYPE_I;
    pic->key_frame = 1;

    y = (uint16_t *)pic->data[0];
    u = (uint16_t *)pic->data[1];
    v = (uint16_t *)pic->data[2];
    line_end = avpkt->data + stride;

    while (line++ < avctx->height) {
        while (1) {
            uint32_t t = AV_RL32(src);
            src += 4;
            *u++ = t <<  6 & 0xFFC0;
            *y++ = t >>  4 & 0xFFC0;
            *v++ = t >> 14 & 0xFFC0;

            if (src >= line_end - 1) {
                *y = 0x80;
                src++;
                line_end += stride;
                y = (uint16_t *)(pic->data[0] + line * pic->linesize[0]);
                u = (uint16_t *)(pic->data[1] + line * pic->linesize[1]);
                v = (uint16_t *)(pic->data[2] + line * pic->linesize[2]);
                break;
            }

            t = AV_RL32(src);
            src += 4;
            *y++ = t <<  6 & 0xFFC0;
            *u++ = t >>  4 & 0xFFC0;
            *y++ = t >> 14 & 0xFFC0;
            if (src >= line_end - 2) {
                if (!(width & 1)) {
                    *y = 0x80;
                    src += 2;
                }
                line_end += stride;
                y = (uint16_t *)(pic->data[0] + line * pic->linesize[0]);
                u = (uint16_t *)(pic->data[1] + line * pic->linesize[1]);
                v = (uint16_t *)(pic->data[2] + line * pic->linesize[2]);
                break;
            }

            t = AV_RL32(src);
            src += 4;
            *v++ = t <<  6 & 0xFFC0;
            *y++ = t >>  4 & 0xFFC0;
            *u++ = t >> 14 & 0xFFC0;

            if (src >= line_end - 1) {
                *y = 0x80;
                src++;
                line_end += stride;
                y = (uint16_t *)(pic->data[0] + line * pic->linesize[0]);
                u = (uint16_t *)(pic->data[1] + line * pic->linesize[1]);
                v = (uint16_t *)(pic->data[2] + line * pic->linesize[2]);
                break;
            }

            t = AV_RL32(src);
            src += 4;
            *y++ = t <<  6 & 0xFFC0;
            *v++ = t >>  4 & 0xFFC0;
            *y++ = t >> 14 & 0xFFC0;

            if (src >= line_end - 2) {
                if (width & 1) {
                    *y = 0x80;
                    src += 2;
                }
                line_end += stride;
                y = (uint16_t *)(pic->data[0] + line * pic->linesize[0]);
                u = (uint16_t *)(pic->data[1] + line * pic->linesize[1]);
                v = (uint16_t *)(pic->data[2] + line * pic->linesize[2]);
                break;
            }
        }
    }

    *got_frame = 1;

    return avpkt->size;
}
