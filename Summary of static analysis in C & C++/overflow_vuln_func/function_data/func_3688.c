static int decode_frame(AVCodecContext *avctx,
                        void *data, int *data_size,
                        AVPacket *avpkt)
{
    J2kDecoderContext *s = avctx->priv_data;
    AVFrame *picture = data;
    int tileno, ret;

    s->avctx = avctx;
    av_log(s->avctx, AV_LOG_DEBUG, "start\n");

    // init
    s->buf = s->buf_start = avpkt->data;
    s->buf_end = s->buf_start + avpkt->size;
    s->curtileno = -1;

    ff_j2k_init_tier1_luts();

    if (s->buf_end - s->buf < 2)
        return AVERROR(EINVAL);

    // check if the image is in jp2 format
    if((AV_RB32(s->buf) == 12) && (AV_RB32(s->buf + 4) == JP2_SIG_TYPE) &&
       (AV_RB32(s->buf + 8) == JP2_SIG_VALUE)) {
        if(!jp2_find_codestream(s)) {
            av_log(avctx, AV_LOG_ERROR, "couldn't find jpeg2k codestream atom\n");
            return -1;
        }
    }

    if (bytestream_get_be16(&s->buf) != J2K_SOC){
        av_log(avctx, AV_LOG_ERROR, "SOC marker not present\n");
        return -1;
    }
    if (ret = decode_codestream(s))
        return ret;

    for (tileno = 0; tileno < s->numXtiles * s->numYtiles; tileno++)
        if (ret = decode_tile(s, s->tile + tileno))
            return ret;

    cleanup(s);
    av_log(s->avctx, AV_LOG_DEBUG, "end\n");

    *data_size = sizeof(AVPicture);
    *picture = s->picture;

    return s->buf - s->buf_start;
}
