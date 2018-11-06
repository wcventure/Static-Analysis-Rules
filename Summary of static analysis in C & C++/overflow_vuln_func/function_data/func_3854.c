static av_cold int msrle_decode_init(AVCodecContext *avctx)
{
    MsrleContext *s = avctx->priv_data;
    int i;

    s->avctx = avctx;

    switch (avctx->bits_per_coded_sample) {
    case 1:
        avctx->pix_fmt = AV_PIX_FMT_MONOWHITE;
        break;
    case 4:
    case 8:
        avctx->pix_fmt = AV_PIX_FMT_PAL8;
        break;
    case 24:
        avctx->pix_fmt = AV_PIX_FMT_BGR24;
        break;
    default:
        av_log(avctx, AV_LOG_ERROR, "unsupported bits per sample\n");
        return -1;
    }

    avcodec_get_frame_defaults(&s->frame);
    s->frame.data[0] = NULL;

    if (avctx->extradata_size >= AVPALETTE_SIZE)
        for (i = 0; i < AVPALETTE_SIZE/4; i++)
            s->pal[i] = 0xFF<<24 | AV_RL32(avctx->extradata+4*i);

    return 0;
}
