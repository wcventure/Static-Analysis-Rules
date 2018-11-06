static int tgv_decode_frame(AVCodecContext *avctx,
                            void *data, int *data_size,
                            AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    TgvContext *s = avctx->priv_data;
    const uint8_t *buf_end = buf + buf_size;
    int chunk_type;

    if (buf_end - buf < EA_PREAMBLE_SIZE)
        return AVERROR_INVALIDDATA;

    chunk_type = AV_RL32(&buf[0]);
    buf += EA_PREAMBLE_SIZE;

    if (chunk_type==kVGT_TAG) {
        int pal_count, i;
        if(buf_end - buf < 12) {
            av_log(avctx, AV_LOG_WARNING, "truncated header\n");
            return -1;
        }

        s->width  = AV_RL16(&buf[0]);
        s->height = AV_RL16(&buf[2]);
        if (s->avctx->width!=s->width || s->avctx->height!=s->height) {
            avcodec_set_dimensions(s->avctx, s->width, s->height);
            cond_release_buffer(&s->frame);
            cond_release_buffer(&s->last_frame);
        }

        pal_count = AV_RL16(&buf[6]);
        buf += 12;
        for(i=0; i<pal_count && i<AVPALETTE_COUNT && buf_end - buf >= 3; i++) {
            s->palette[i] = 0xFF << 24 | AV_RB24(buf);
            buf += 3;
        }
    }

    if (av_image_check_size(s->width, s->height, 0, avctx))
        return -1;

    /
    FFSWAP(AVFrame, s->frame, s->last_frame);
    if (!s->frame.data[0]) {
        s->frame.reference = 3;
        s->frame.buffer_hints = FF_BUFFER_HINTS_VALID;
        s->frame.linesize[0] = s->width;

        /
        s->frame.data[0] = av_malloc(s->width*s->height + 12);
        if (!s->frame.data[0])
            return AVERROR(ENOMEM);
        s->frame.data[1] = av_malloc(AVPALETTE_SIZE);
        if (!s->frame.data[1]) {
            av_freep(&s->frame.data[0]);
            return AVERROR(ENOMEM);
        }
    }
    memcpy(s->frame.data[1], s->palette, AVPALETTE_SIZE);

    if(chunk_type==kVGT_TAG) {
        s->frame.key_frame = 1;
        s->frame.pict_type = AV_PICTURE_TYPE_I;
        if (unpack(buf, buf_end, s->frame.data[0], s->avctx->width, s->avctx->height)<0) {
            av_log(avctx, AV_LOG_WARNING, "truncated intra frame\n");
            return -1;
        }
    }else{
        if (!s->last_frame.data[0]) {
            av_log(avctx, AV_LOG_WARNING, "inter frame without corresponding intra frame\n");
            return buf_size;
        }
        s->frame.key_frame = 0;
        s->frame.pict_type = AV_PICTURE_TYPE_P;
        if (tgv_decode_inter(s, buf, buf_end)<0) {
            av_log(avctx, AV_LOG_WARNING, "truncated inter frame\n");
            return -1;
        }
    }

    *data_size = sizeof(AVFrame);
    *(AVFrame*)data = s->frame;

    return buf_size;
}
