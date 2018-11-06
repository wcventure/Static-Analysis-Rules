static int decode_frame(AVCodecContext *avctx,
                        void *data, int *data_size,
                        AVPacket *avpkt)
{
    JvContext *s           = avctx->priv_data;
    int buf_size           = avpkt->size;
    const uint8_t *buf     = avpkt->data;
    const uint8_t *buf_end = buf + buf_size;
    int video_size, video_type, i, j;

    video_size = AV_RL32(buf);
    video_type = buf[4];
    buf += 5;

    if (video_size) {
        if(video_size < 0) {
            av_log(avctx, AV_LOG_ERROR, "video size %d invalid\n", video_size);
            return AVERROR_INVALIDDATA;
        }
        if (avctx->reget_buffer(avctx, &s->frame) < 0) {
            av_log(avctx, AV_LOG_ERROR, "get_buffer() failed\n");
            return -1;
        }

        if (video_type == 0 || video_type == 1) {
            GetBitContext gb;
            init_get_bits(&gb, buf, 8 * FFMIN(video_size, buf_end - buf));

            for (j = 0; j < avctx->height; j += 8)
                for (i = 0; i < avctx->width; i += 8)
                    decode8x8(&gb, s->frame.data[0] + j*s->frame.linesize[0] + i,
                              s->frame.linesize[0], &s->dsp);

            buf += video_size;
        } else if (video_type == 2) {
            if (buf + 1 <= buf_end) {
                int v = *buf++;
                for (j = 0; j < avctx->height; j++)
                    memset(s->frame.data[0] + j*s->frame.linesize[0], v, avctx->width);
            }
        } else {
            av_log(avctx, AV_LOG_WARNING, "unsupported frame type %i\n", video_type);
            return AVERROR_INVALIDDATA;
        }
    }

    if (buf < buf_end) {
        for (i = 0; i < AVPALETTE_COUNT && buf + 3 <= buf_end; i++) {
            uint32_t pal = AV_RB24(buf);
            s->palette[i] = 0xFF << 24 | pal << 2 | ((pal >> 4) & 0x30303);
            buf += 3;
        }
        s->palette_has_changed = 1;
    }

    if (video_size) {
        s->frame.key_frame           = 1;
        s->frame.pict_type           = AV_PICTURE_TYPE_I;
        s->frame.palette_has_changed = s->palette_has_changed;
        s->palette_has_changed       = 0;
        memcpy(s->frame.data[1], s->palette, AVPALETTE_SIZE);

        *data_size      = sizeof(AVFrame);
        *(AVFrame*)data = s->frame;
    }

    return buf_size;
}
