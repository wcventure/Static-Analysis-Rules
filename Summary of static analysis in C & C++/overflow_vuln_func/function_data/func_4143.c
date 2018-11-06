static int decode_frame(AVCodecContext *avctx,
                        void *data, int *got_frame,
                        AVPacket *avpkt)
{
    FMVCContext *s = avctx->priv_data;
    GetByteContext *gb = &s->gb;
    PutByteContext *pb = &s->pb;
    AVFrame *frame = data;
    int ret, y, x;

    if ((ret = ff_get_buffer(avctx, frame, 0)) < 0)
        return ret;

    bytestream2_init(gb, avpkt->data, avpkt->size);
    bytestream2_skip(gb, 2);

    frame->key_frame = !!bytestream2_get_le16(gb);
    frame->pict_type = frame->key_frame ? AV_PICTURE_TYPE_I : AV_PICTURE_TYPE_P;

    if (frame->key_frame) {
        const uint8_t *src;
        int type, size;
        uint8_t *dst;

        type = bytestream2_get_le16(gb);
        size = bytestream2_get_le16(gb);
        if (size > bytestream2_get_bytes_left(gb))
            return AVERROR_INVALIDDATA;

        bytestream2_init_writer(pb, s->buffer, s->buffer_size);
        if (type == 1) {
            decode_type1(gb, pb);
        } else if (type == 2){
            decode_type2(gb, pb);
        } else {
            avpriv_report_missing_feature(avctx, "compression %d", type);
            return AVERROR_PATCHWELCOME;
        }

        src = s->buffer;
        dst = frame->data[0] + (avctx->height - 1) * frame->linesize[0];
        for (y = 0; y < avctx->height; y++) {
            memcpy(dst, src, avctx->width * s->bpp);
            dst -= frame->linesize[0];
            src += s->stride * 4;
        }
    } else {
        int block, nb_blocks, type, k, l;
        uint8_t *ssrc, *ddst;
        const uint32_t *src;
        uint32_t *dst;

        for (block = 0; block < s->nb_blocks; block++)
            s->blocks[block].xor = 0;

        nb_blocks = bytestream2_get_le16(gb);
        if (nb_blocks > s->nb_blocks)
            return AVERROR_INVALIDDATA;

        bytestream2_init_writer(pb, s->pbuffer, s->pbuffer_size);

        type = bytestream2_get_le16(gb);
        for (block = 0; block < nb_blocks; block++) {
            int size, offset, start = 0;

            offset = bytestream2_get_le16(gb);
            if (offset > s->nb_blocks)
                return AVERROR_INVALIDDATA;

            size = bytestream2_get_le16(gb);
            if (size > bytestream2_get_bytes_left(gb))
                return AVERROR_INVALIDDATA;

            start = bytestream2_tell_p(pb);
            if (type == 1) {
                decode_type1(gb, pb);
            } else if (type == 2){
                decode_type2(gb, pb);
            } else {
                avpriv_report_missing_feature(avctx, "compression %d", type);
                return AVERROR_PATCHWELCOME;
            }

            if (s->blocks[offset].size * 4 != bytestream2_tell_p(pb) - start)
                return AVERROR_INVALIDDATA;

            s->blocks[offset].xor = 1;
        }

        src = (const uint32_t *)s->pbuffer;
        dst = (uint32_t *)s->buffer;

        for (block = 0, y = 0; y < s->yb; y++) {
            int block_h = s->blocks[block].h;
            uint32_t *rect = dst;

            for (x = 0; x < s->xb; x++) {
                int block_w = s->blocks[block].w;
                uint32_t *row = dst;

                block_h = s->blocks[block].h;
                if (s->blocks[block].xor) {
                    for (k = 0; k < block_h; k++) {
                        uint32_t *column = dst;
                        for (l = 0; l < block_w; l++) {
                            *dst++ ^= *src++;
                        }
                        dst = &column[s->stride];
                    }
                }
                dst = &row[block_w];
                ++block;
            }
            dst = &rect[block_h * s->stride];
        }

        ssrc = s->buffer;
        ddst = frame->data[0] + (avctx->height - 1) * frame->linesize[0];
        for (y = 0; y < avctx->height; y++) {
            memcpy(ddst, ssrc, avctx->width * s->bpp);
            ddst -= frame->linesize[0];
            ssrc += s->stride * 4;
        }
    }

    *got_frame = 1;

    return avpkt->size;
}
