static int decode_frame(AVCodecContext *avctx, void *data, int *data_size, AVPacket *pkt)
{
    BMVDecContext * const c = avctx->priv_data;
    int type, scr_off;
    int i;
    uint8_t *srcptr, *outptr;

    c->stream = pkt->data;
    type = bytestream_get_byte(&c->stream);
    if (type & BMV_AUDIO) {
        int blobs = bytestream_get_byte(&c->stream);
        if (pkt->size < blobs * 65 + 2) {
            av_log(avctx, AV_LOG_ERROR, "Audio data doesn't fit in frame\n");
            return AVERROR_INVALIDDATA;
        }
        c->stream += blobs * 65;
    }
    if (type & BMV_COMMAND) {
        int command_size = (type & BMV_PRINT) ? 8 : 10;
        if (c->stream - pkt->data + command_size > pkt->size) {
            av_log(avctx, AV_LOG_ERROR, "Command data doesn't fit in frame\n");
            return AVERROR_INVALIDDATA;
        }
        c->stream += command_size;
    }
    if (type & BMV_PALETTE) {
        if (c->stream - pkt->data > pkt->size - 768) {
            av_log(avctx, AV_LOG_ERROR, "Palette data doesn't fit in frame\n");
            return AVERROR_INVALIDDATA;
        }
        for (i = 0; i < 256; i++)
            c->pal[i] = 0xFF << 24 | bytestream_get_be24(&c->stream);
    }
    if (type & BMV_SCROLL) {
        if (c->stream - pkt->data > pkt->size - 2) {
            av_log(avctx, AV_LOG_ERROR, "Screen offset data doesn't fit in frame\n");
            return AVERROR_INVALIDDATA;
        }
        scr_off = (int16_t)bytestream_get_le16(&c->stream);
    } else if ((type & BMV_INTRA) == BMV_INTRA) {
        scr_off = -640;
    } else {
        scr_off = 0;
    }

    if (decode_bmv_frame(c->stream, pkt->size - (c->stream - pkt->data), c->frame, scr_off)) {
        av_log(avctx, AV_LOG_ERROR, "Error decoding frame data\n");
        return AVERROR_INVALIDDATA;
    }

    memcpy(c->pic.data[1], c->pal, AVPALETTE_SIZE);
    c->pic.palette_has_changed = type & BMV_PALETTE;

    outptr = c->pic.data[0];
    srcptr = c->frame;

    for (i = 0; i < avctx->height; i++) {
        memcpy(outptr, srcptr, avctx->width);
        srcptr += avctx->width;
        outptr += c->pic.linesize[0];
    }

    *data_size = sizeof(AVFrame);
    *(AVFrame*)data = c->pic;

    /
    return pkt->size;
}
