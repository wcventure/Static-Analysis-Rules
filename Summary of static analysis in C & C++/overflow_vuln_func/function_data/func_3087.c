static int dfa_decode_frame(AVCodecContext *avctx,
                            void *data, int *data_size,
                            AVPacket *avpkt)
{
    DfaContext *s = avctx->priv_data;
    GetByteContext gb;
    const uint8_t *buf = avpkt->data;
    uint32_t chunk_type, chunk_size;
    uint8_t *dst;
    int ret;
    int i, pal_elems;

    if (s->pic.data[0])
        avctx->release_buffer(avctx, &s->pic);

    if ((ret = avctx->get_buffer(avctx, &s->pic))) {
        av_log(avctx, AV_LOG_ERROR, "get_buffer() failed\n");
        return ret;
    }

    bytestream2_init(&gb, avpkt->data, avpkt->size);
    while (bytestream2_get_bytes_left(&gb) > 0) {
        bytestream2_skip(&gb, 4);
        chunk_size = bytestream2_get_le32(&gb);
        chunk_type = bytestream2_get_le32(&gb);
        if (!chunk_type)
            break;
        if (chunk_type == 1) {
            pal_elems = FFMIN(chunk_size / 3, 256);
            for (i = 0; i < pal_elems; i++) {
                s->pal[i] = bytestream2_get_be24(&gb) << 2;
                s->pal[i] |= 0xFF << 24 | (s->pal[i] >> 6) & 0x30303;
            }
            s->pic.palette_has_changed = 1;
        } else if (chunk_type <= 9) {
            if (decoder[chunk_type - 2](&gb, s->frame_buf, avctx->width, avctx->height)) {
                av_log(avctx, AV_LOG_ERROR, "Error decoding %s chunk\n",
                       chunk_name[chunk_type - 2]);
                return AVERROR_INVALIDDATA;
            }
        } else {
            av_log(avctx, AV_LOG_WARNING, "Ignoring unknown chunk type %d\n",
                   chunk_type);
        }
        buf += chunk_size;
    }

    buf = s->frame_buf;
    dst = s->pic.data[0];
    for (i = 0; i < avctx->height; i++) {
        memcpy(dst, buf, avctx->width);
        dst += s->pic.linesize[0];
        buf += avctx->width;
    }
    memcpy(s->pic.data[1], s->pal, sizeof(s->pal));

    *data_size = sizeof(AVFrame);
    *(AVFrame*)data = s->pic;

    return avpkt->size;
}
