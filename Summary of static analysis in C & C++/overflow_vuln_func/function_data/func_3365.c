static int ipvideo_decode_frame(AVCodecContext *avctx,
                                void *data, int *got_frame,
                                AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    IpvideoContext *s = avctx->priv_data;
    AVFrame *frame = data;
    int ret;

    if (buf_size < 2)
        return AVERROR_INVALIDDATA;

    /
    s->decoding_map_size = AV_RL16(avpkt->data);

    /
    if (buf_size < s->decoding_map_size + 2)
        return buf_size;

    if (av_packet_get_side_data(avpkt, AV_PKT_DATA_PARAM_CHANGE, NULL)) {
        av_frame_unref(s->last_frame);
        av_frame_unref(s->second_last_frame);
    }

    s->decoding_map = buf + 2;
    bytestream2_init(&s->stream_ptr, buf + 2 + s->decoding_map_size,
                     buf_size - s->decoding_map_size);

    if ((ret = ff_get_buffer(avctx, frame, AV_GET_BUFFER_FLAG_REF)) < 0)
        return ret;

    if (!s->is_16bpp) {
        int size;
        const uint8_t *pal = av_packet_get_side_data(avpkt, AV_PKT_DATA_PALETTE, &size);
        if (pal && size == AVPALETTE_SIZE) {
            frame->palette_has_changed = 1;
            memcpy(s->pal, pal, AVPALETTE_SIZE);
        } else if (pal) {
            av_log(avctx, AV_LOG_ERROR, "Palette size %d is wrong\n", size);
        }
    }

    ipvideo_decode_opcodes(s, frame);

    *got_frame = 1;

    /
    av_frame_unref(s->second_last_frame);
    FFSWAP(AVFrame*, s->second_last_frame, s->last_frame);
    if ((ret = av_frame_ref(s->last_frame, frame)) < 0)
        return ret;

    /
    return buf_size;
}
