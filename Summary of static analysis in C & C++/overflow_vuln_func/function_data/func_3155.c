static av_cold int decode_init(AVCodecContext *avctx)
{
    InterplayACMContext *s = avctx->priv_data;
    int x1, x2, x3;

    if (avctx->extradata_size < 14)
        return AVERROR_INVALIDDATA;

    if (avctx->channels <= 0) {
        av_log(avctx, AV_LOG_ERROR, "Invalid number of channels: %d\n", avctx->channels);
        return AVERROR_INVALIDDATA;
    }

    s->level = AV_RL16(avctx->extradata + 12) & 0xf;
    s->rows  = AV_RL16(avctx->extradata + 12) >>  4;
    s->cols  = 1 << s->level;
    s->wrapbuf_len = 2 * s->cols - 2;
    s->block_len = s->rows * s->cols;
    s->max_framesize = s->block_len;

    s->block   = av_calloc(s->block_len, sizeof(int));
    s->wrapbuf = av_calloc(s->wrapbuf_len, sizeof(int));
    s->ampbuf  = av_calloc(0x10000, sizeof(int));
    s->bitstream = av_calloc(s->max_framesize, sizeof(*s->bitstream));
    if (!s->block || !s->wrapbuf || !s->ampbuf || !s->bitstream)
        return AVERROR(ENOMEM);

    s->midbuf  = s->ampbuf + 0x8000;
    avctx->sample_fmt = AV_SAMPLE_FMT_S16;

    for (x3 = 0; x3 < 3; x3++)
        for (x2 = 0; x2 < 3; x2++)
            for (x1 = 0; x1 < 3; x1++)
                mul_3x3[x1 + x2 * 3 + x3* 3 * 3] = x1 + (x2 << 4) + (x3 << 8);
    for (x3 = 0; x3 < 5; x3++)
        for (x2 = 0; x2 < 5; x2++)
            for (x1 = 0; x1 < 5; x1++)
                mul_3x5[x1 + x2 * 5 + x3 * 5 * 5] = x1 + (x2 << 4) + (x3 << 8);
    for (x2 = 0; x2 < 11; x2++)
        for (x1 = 0; x1 < 11; x1++)
            mul_2x11[x1 + x2 * 11] = x1 + (x2 << 4);

    return 0;
}
