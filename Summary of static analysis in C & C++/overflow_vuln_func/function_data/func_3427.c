static int xan_decode_chroma(AVCodecContext *avctx, AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    XanContext *s = avctx->priv_data;
    uint8_t *U, *V;
    unsigned chroma_off;
    int val, uval, vval;
    int i, j;
    const uint8_t *src, *src_end;
    const uint8_t *table;
    int mode, offset, dec_size;

    chroma_off = AV_RL32(buf + 4);
    if (!chroma_off)
        return 0;
    if (chroma_off + 10 >= avpkt->size) {
        av_log(avctx, AV_LOG_ERROR, "Invalid chroma block position\n");
        return -1;
    }
    src    = avpkt->data + 4 + chroma_off;
    table  = src + 2;
    mode   = bytestream_get_le16(&src);
    offset = bytestream_get_le16(&src) * 2;

    if (src - avpkt->data >= avpkt->size - offset) {
        av_log(avctx, AV_LOG_ERROR, "Invalid chroma block offset\n");
        return -1;
    }

    memset(s->scratch_buffer, 0, s->buffer_size);
    dec_size = xan_unpack(s->scratch_buffer, s->buffer_size, src + offset,
                          avpkt->size - offset - (src - avpkt->data));
    if (dec_size < 0) {
        av_log(avctx, AV_LOG_ERROR, "Chroma unpacking failed\n");
        return -1;
    }

    U = s->pic.data[1];
    V = s->pic.data[2];
    src     = s->scratch_buffer;
    src_end = src + dec_size;
    if (mode) {
        for (j = 0; j < avctx->height >> 1; j++) {
            for (i = 0; i < avctx->width >> 1; i++) {
                val = *src++;
                if (val) {
                    val  = AV_RL16(table + (val << 1));
                    uval = (val >> 3) & 0xF8;
                    vval = (val >> 8) & 0xF8;
                    U[i] = uval | (uval >> 5);
                    V[i] = vval | (vval >> 5);
                }
                if (src == src_end)
                    return 0;
            }
            U += s->pic.linesize[1];
            V += s->pic.linesize[2];
        }
    } else {
        uint8_t *U2 = U + s->pic.linesize[1];
        uint8_t *V2 = V + s->pic.linesize[2];

        for (j = 0; j < avctx->height >> 2; j++) {
            for (i = 0; i < avctx->width >> 1; i += 2) {
                val = *src++;
                if (val) {
                    val  = AV_RL16(table + (val << 1));
                    uval = (val >> 3) & 0xF8;
                    vval = (val >> 8) & 0xF8;
                    U[i] = U[i+1] = U2[i] = U2[i+1] = uval | (uval >> 5);
                    V[i] = V[i+1] = V2[i] = V2[i+1] = vval | (vval >> 5);
                }
            }
            U  += s->pic.linesize[1] * 2;
            V  += s->pic.linesize[2] * 2;
            U2 += s->pic.linesize[1] * 2;
            V2 += s->pic.linesize[2] * 2;
        }
    }

    return 0;
}
