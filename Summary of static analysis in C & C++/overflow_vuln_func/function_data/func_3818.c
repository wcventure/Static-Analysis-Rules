static int decode_frame(AVCodecContext *avctx,
                        void *data, int *got_frame,
                        AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size       = avpkt->size;
    MadContext *s     = avctx->priv_data;
    AVFrame *frame    = data;
    GetByteContext gb;
    int width, height;
    int chunk_type;
    int inter, ret;

    bytestream2_init(&gb, buf, buf_size);

    chunk_type = bytestream2_get_le32(&gb);
    inter = (chunk_type == MADm_TAG || chunk_type == MADe_TAG);
    bytestream2_skip(&gb, 10);

    av_reduce(&avctx->framerate.den, &avctx->framerate.num,
              bytestream2_get_le16(&gb), 1000, 1<<30);

    width  = bytestream2_get_le16(&gb);
    height = bytestream2_get_le16(&gb);
    bytestream2_skip(&gb, 1);
    calc_quant_matrix(s, bytestream2_get_byte(&gb));
    bytestream2_skip(&gb, 2);

    if (bytestream2_get_bytes_left(&gb) < 2) {
        av_log(avctx, AV_LOG_ERROR, "Input data too small\n");
        return AVERROR_INVALIDDATA;
    }

    if (width < 16 || height < 16) {
        av_log(avctx, AV_LOG_ERROR, "Dimensions too small\n");
        return AVERROR_INVALIDDATA;
    }

    if (avctx->width != width || avctx->height != height) {
        av_frame_unref(s->last_frame);
        if((width * height)/2048*7 > bytestream2_get_bytes_left(&gb))
            return AVERROR_INVALIDDATA;
        if ((ret = ff_set_dimensions(avctx, width, height)) < 0)
            return ret;
    }

    if ((ret = ff_get_buffer(avctx, frame, AV_GET_BUFFER_FLAG_REF)) < 0)
        return ret;

    if (inter && !s->last_frame->data[0]) {
        av_log(avctx, AV_LOG_WARNING, "Missing reference frame.\n");
        ret = ff_get_buffer(avctx, s->last_frame, AV_GET_BUFFER_FLAG_REF);
        if (ret < 0)
            return ret;
        memset(s->last_frame->data[0], 0, s->last_frame->height *
               s->last_frame->linesize[0]);
        memset(s->last_frame->data[1], 0x80, s->last_frame->height / 2 *
               s->last_frame->linesize[1]);
        memset(s->last_frame->data[2], 0x80, s->last_frame->height / 2 *
               s->last_frame->linesize[2]);
    }

    av_fast_padded_malloc(&s->bitstream_buf, &s->bitstream_buf_size,
                          bytestream2_get_bytes_left(&gb));
    if (!s->bitstream_buf)
        return AVERROR(ENOMEM);
    s->bbdsp.bswap16_buf(s->bitstream_buf, (const uint16_t *)(buf + bytestream2_tell(&gb)),
                         bytestream2_get_bytes_left(&gb) / 2);
    memset((uint8_t*)s->bitstream_buf + bytestream2_get_bytes_left(&gb), 0, AV_INPUT_BUFFER_PADDING_SIZE);
    init_get_bits(&s->gb, s->bitstream_buf, 8*(bytestream2_get_bytes_left(&gb)));

    for (s->mb_y=0; s->mb_y < (avctx->height+15)/16; s->mb_y++)
        for (s->mb_x=0; s->mb_x < (avctx->width +15)/16; s->mb_x++)
            if(decode_mb(s, frame, inter) < 0)
                return AVERROR_INVALIDDATA;

    *got_frame = 1;

    if (chunk_type != MADe_TAG) {
        av_frame_unref(s->last_frame);
        if ((ret = av_frame_ref(s->last_frame, frame)) < 0)
            return ret;
    }

    return buf_size;
}
