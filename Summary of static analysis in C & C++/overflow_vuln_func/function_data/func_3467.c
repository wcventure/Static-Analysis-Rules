static int decode_wmv9(AVCodecContext *avctx, const uint8_t *buf, int buf_size,
                       int x, int y, int w, int h, int wmv9_mask)
{
    MSS2Context *ctx  = avctx->priv_data;
    MSS12Context *c   = &ctx->c;
    VC1Context *v     = avctx->priv_data;
    MpegEncContext *s = &v->s;
    AVFrame *f;
    int ret;

    ff_mpeg_flush(avctx);

    init_get_bits(&s->gb, buf, buf_size * 8);

    s->loop_filter = avctx->skip_loop_filter < AVDISCARD_ALL;

    if (ff_vc1_parse_frame_header(v, &s->gb) < 0) {
        av_log(v->s.avctx, AV_LOG_ERROR, "header error\n");
        return AVERROR_INVALIDDATA;
    }

    if (s->pict_type != AV_PICTURE_TYPE_I) {
        av_log(v->s.avctx, AV_LOG_ERROR, "expected I-frame\n");
        return AVERROR_INVALIDDATA;
    }

    avctx->pix_fmt = AV_PIX_FMT_YUV420P;

    if ((ret = ff_mpv_frame_start(s, avctx)) < 0) {
        av_log(v->s.avctx, AV_LOG_ERROR, "ff_mpv_frame_start error\n");
        avctx->pix_fmt = AV_PIX_FMT_RGB24;
        return ret;
    }

    ff_mpeg_er_frame_start(s);

    v->bits = buf_size * 8;

    v->end_mb_x = (w + 15) >> 4;
    s->end_mb_y = (h + 15) >> 4;
    if (v->respic & 1)
        v->end_mb_x = v->end_mb_x + 1 >> 1;
    if (v->respic & 2)
        s->end_mb_y = s->end_mb_y + 1 >> 1;

    ff_vc1_decode_blocks(v);

    ff_er_frame_end(&s->er);

    ff_mpv_frame_end(s);

    f = s->current_picture.f;

    if (v->respic == 3) {
        ctx->dsp.upsample_plane(f->data[0], f->linesize[0], w,      h);
        ctx->dsp.upsample_plane(f->data[1], f->linesize[1], w >> 1, h >> 1);
        ctx->dsp.upsample_plane(f->data[2], f->linesize[2], w >> 1, h >> 1);
    } else if (v->respic)
        avpriv_request_sample(v->s.avctx,
                              "Asymmetric WMV9 rectangle subsampling");

    av_assert0(f->linesize[1] == f->linesize[2]);

    if (wmv9_mask != -1)
        ctx->dsp.mss2_blit_wmv9_masked(c->rgb_pic + y * c->rgb_stride + x * 3,
                                       c->rgb_stride, wmv9_mask,
                                       c->pal_pic + y * c->pal_stride + x,
                                       c->pal_stride,
                                       f->data[0], f->linesize[0],
                                       f->data[1], f->data[2], f->linesize[1],
                                       w, h);
    else
        ctx->dsp.mss2_blit_wmv9(c->rgb_pic + y * c->rgb_stride + x * 3,
                                c->rgb_stride,
                                f->data[0], f->linesize[0],
                                f->data[1], f->data[2], f->linesize[1],
                                w, h);

    avctx->pix_fmt = AV_PIX_FMT_RGB24;

    return 0;
}
