static int update_size(AVCodecContext *ctx, int w, int h, enum AVPixelFormat fmt)
{
    VP9Context *s = ctx->priv_data;
    uint8_t *p;
    int bytesperpixel = s->bytesperpixel;

    av_assert0(w > 0 && h > 0);

    if (s->intra_pred_data[0] && w == ctx->width && h == ctx->height && ctx->pix_fmt == fmt)
        return 0;

    ctx->width   = w;
    ctx->height  = h;
    ctx->pix_fmt = fmt;
    s->sb_cols   = (w + 63) >> 6;
    s->sb_rows   = (h + 63) >> 6;
    s->cols      = (w + 7) >> 3;
    s->rows      = (h + 7) >> 3;

#define assign(var, type, n) var = (type) p; p += s->sb_cols * (n) * sizeof(*var)
    av_freep(&s->intra_pred_data[0]);
    // FIXME we slightly over-allocate here for subsampled chroma, but a little
    // bit of padding shouldn't affect performance...
    p = av_malloc(s->sb_cols * (128 + 192 * bytesperpixel +
                                sizeof(*s->lflvl) + 16 * sizeof(*s->above_mv_ctx)));
    if (!p)
        return AVERROR(ENOMEM);
    assign(s->intra_pred_data[0],  uint8_t *,             64 * bytesperpixel);
    assign(s->intra_pred_data[1],  uint8_t *,             64 * bytesperpixel);
    assign(s->intra_pred_data[2],  uint8_t *,             64 * bytesperpixel);
    assign(s->above_y_nnz_ctx,     uint8_t *,             16);
    assign(s->above_mode_ctx,      uint8_t *,             16);
    assign(s->above_mv_ctx,        VP56mv(*)[2],          16);
    assign(s->above_uv_nnz_ctx[0], uint8_t *,             16);
    assign(s->above_uv_nnz_ctx[1], uint8_t *,             16);
    assign(s->above_partition_ctx, uint8_t *,              8);
    assign(s->above_skip_ctx,      uint8_t *,              8);
    assign(s->above_txfm_ctx,      uint8_t *,              8);
    assign(s->above_segpred_ctx,   uint8_t *,              8);
    assign(s->above_intra_ctx,     uint8_t *,              8);
    assign(s->above_comp_ctx,      uint8_t *,              8);
    assign(s->above_ref_ctx,       uint8_t *,              8);
    assign(s->above_filter_ctx,    uint8_t *,              8);
    assign(s->lflvl,               struct VP9Filter *,     1);
#undef assign

    // these will be re-allocated a little later
    av_freep(&s->b_base);
    av_freep(&s->block_base);

    if (s->bpp != s->last_bpp) {
        ff_vp9dsp_init(&s->dsp, s->bpp);
        ff_videodsp_init(&s->vdsp, s->bpp);
        s->last_bpp = s->bpp;
    }

    return 0;
}
