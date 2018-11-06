static int seqvideo_decode_frame(AVCodecContext *avctx,
                                 void *data, int *data_size,
                                 AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;

    SeqVideoContext *seq = avctx->priv_data;

    seq->frame.reference = 1;
    seq->frame.buffer_hints = FF_BUFFER_HINTS_VALID | FF_BUFFER_HINTS_PRESERVE | FF_BUFFER_HINTS_REUSABLE;
    if (avctx->reget_buffer(avctx, &seq->frame)) {
        av_log(seq->avctx, AV_LOG_ERROR, "tiertexseqvideo: reget_buffer() failed\n");
        return -1;
    }

    seqvideo_decode(seq, buf, buf_size);

    *data_size = sizeof(AVFrame);
    *(AVFrame *)data = seq->frame;

    return buf_size;
}
