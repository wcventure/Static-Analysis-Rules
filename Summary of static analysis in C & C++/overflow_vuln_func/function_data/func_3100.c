static av_cold int decode_init(AVCodecContext *avctx)
{
    SANMVideoContext *ctx = avctx->priv_data;

    ctx->avctx     = avctx;
    ctx->version   = !avctx->extradata_size;

    avctx->pix_fmt = ctx->version ? AV_PIX_FMT_RGB565 : AV_PIX_FMT_PAL8;

    init_sizes(ctx, avctx->width, avctx->height);
    if (init_buffers(ctx)) {
        av_log(avctx, AV_LOG_ERROR, "error allocating buffers\n");
        return AVERROR(ENOMEM);
    }
    ctx->output          = &ctx->frame;
    ctx->output->data[0] = 0;

    make_glyphs(ctx->p4x4glyphs[0], glyph4_x, glyph4_y, 4);
    make_glyphs(ctx->p8x8glyphs[0], glyph8_x, glyph8_y, 8);

    if (!ctx->version) {
        int i;

        if (avctx->extradata_size < 1026) {
            av_log(avctx, AV_LOG_ERROR, "not enough extradata\n");
            return AVERROR_INVALIDDATA;
        }

        ctx->subversion = AV_RL16(avctx->extradata);
        for (i = 0; i < 256; i++)
            ctx->pal[i] = 0xFF << 24 | AV_RL32(avctx->extradata + 2 + i * 4);
    }

    return 0;
}
