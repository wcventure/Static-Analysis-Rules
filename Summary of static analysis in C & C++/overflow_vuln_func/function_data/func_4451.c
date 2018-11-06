static int bitpacked_decode_yuv422p10(AVCodecContext *avctx, AVFrame *frame,
                                      AVPacket *avpkt)
{
    uint64_t frame_size = (uint64_t)avctx->width * (uint64_t)avctx->height * 20;
    uint64_t packet_size = avpkt->size * 8;
    GetBitContext bc;
    uint16_t *y, *u, *v;
    int ret, i;

    ret = ff_get_buffer(avctx, frame, 0);
    if (ret < 0)
        return ret;

    y = (uint16_t*)frame->data[0];
    u = (uint16_t*)frame->data[1];
    v = (uint16_t*)frame->data[2];

    if (frame_size > packet_size)
        return AVERROR_INVALIDDATA;

    if (avctx->width % 2)
        return AVERROR_PATCHWELCOME;

    ret = init_get_bits(&bc, avpkt->data, avctx->width * avctx->height * 20);
    if (ret)
        return ret;

    for (i = 0; i < avctx->height; i++) {
        y = (uint16_t*)(frame->data[0] + i * frame->linesize[0]);
        u = (uint16_t*)(frame->data[1] + i * frame->linesize[1]);
        v = (uint16_t*)(frame->data[2] + i * frame->linesize[2]);

        for (int j = 0; j < avctx->width; j += 2) {
            *u++ = get_bits(&bc, 10);
            *y++ = get_bits(&bc, 10);
            *v++ = get_bits(&bc, 10);
            *y++ = get_bits(&bc, 10);
        }
    }

    return 0;
}
