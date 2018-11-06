static int hnm_decode_frame(AVCodecContext *avctx, void *data,
                            int *got_frame, AVPacket *avpkt)
{
    AVFrame *frame = data;
    Hnm4VideoContext *hnm = avctx->priv_data;
    int ret;
    uint16_t chunk_id;

    if (avpkt->size < 8) {
        av_log(avctx, AV_LOG_ERROR, "packet too small\n");
        return AVERROR_INVALIDDATA;
    }

    if ((ret = ff_get_buffer(avctx, frame, 0)) < 0)
        return ret;

    chunk_id = AV_RL16(avpkt->data + 4);

    if (chunk_id == HNM4_CHUNK_ID_PL) {
        hnm_update_palette(avctx, avpkt->data, avpkt->size);
        frame->palette_has_changed = 1;
    } else if (chunk_id == HNM4_CHUNK_ID_IZ) {
        if (avpkt->size < 12) {
            av_log(avctx, AV_LOG_ERROR, "packet too small\n");
            return AVERROR_INVALIDDATA;
        }
        unpack_intraframe(avctx, avpkt->data + 12, avpkt->size - 12);
        memcpy(hnm->previous, hnm->current, hnm->width * hnm->height);
        if (hnm->version == 0x4a)
            memcpy(hnm->processed, hnm->current, hnm->width * hnm->height);
        else
            postprocess_current_frame(avctx);
        copy_processed_frame(avctx, frame);
        frame->pict_type = AV_PICTURE_TYPE_I;
        frame->key_frame = 1;
        memcpy(frame->data[1], hnm->palette, 256 * 4);
        *got_frame = 1;
    } else if (chunk_id == HNM4_CHUNK_ID_IU) {
        if (hnm->version == 0x4a) {
            decode_interframe_v4a(avctx, avpkt->data + 8, avpkt->size - 8);
            memcpy(hnm->processed, hnm->current, hnm->width * hnm->height);
        } else {
            decode_interframe_v4(avctx, avpkt->data + 8, avpkt->size - 8);
            postprocess_current_frame(avctx);
        }
        copy_processed_frame(avctx, frame);
        frame->pict_type = AV_PICTURE_TYPE_P;
        frame->key_frame = 0;
        memcpy(frame->data[1], hnm->palette, 256 * 4);
        *got_frame = 1;
        hnm_flip_buffers(hnm);
    } else {
        av_log(avctx, AV_LOG_ERROR, "invalid chunk id: %d\n", chunk_id);
        return AVERROR_INVALIDDATA;
    }

    return avpkt->size;
}
