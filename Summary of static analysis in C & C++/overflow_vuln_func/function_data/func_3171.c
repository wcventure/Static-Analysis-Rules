static int imc_decode_frame(AVCodecContext *avctx, void *data,
                            int *got_frame_ptr, AVPacket *avpkt)
{
    AVFrame *frame     = data;
    const uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    int ret, i;

    IMCContext *q = avctx->priv_data;

    LOCAL_ALIGNED_16(uint16_t, buf16, [IMC_BLOCK_SIZE / 2]);

    if (buf_size < IMC_BLOCK_SIZE * avctx->channels) {
        av_log(avctx, AV_LOG_ERROR, "frame too small!\n");
        return AVERROR_INVALIDDATA;
    }

    /
    frame->nb_samples = COEFFS;
    if ((ret = ff_get_buffer(avctx, frame, 0)) < 0) {
        av_log(avctx, AV_LOG_ERROR, "get_buffer() failed\n");
        return ret;
    }

    for (i = 0; i < avctx->channels; i++) {
        q->out_samples = (float *)frame->extended_data[i];

        q->bdsp.bswap16_buf(buf16, (const uint16_t *) buf, IMC_BLOCK_SIZE / 2);

        init_get_bits(&q->gb, (const uint8_t*)buf16, IMC_BLOCK_SIZE * 8);

        buf += IMC_BLOCK_SIZE;

        if ((ret = imc_decode_block(avctx, q, i)) < 0)
            return ret;
    }

    if (avctx->channels == 2) {
        q->fdsp.butterflies_float((float *)frame->extended_data[0],
                                  (float *)frame->extended_data[1], COEFFS);
    }

    *got_frame_ptr = 1;

    return IMC_BLOCK_SIZE * avctx->channels;
}
