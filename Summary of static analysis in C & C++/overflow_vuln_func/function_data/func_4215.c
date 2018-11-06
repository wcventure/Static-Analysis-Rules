static int update_dimensions(VP8Context *s, int width, int height)
{
    if (width  != s->avctx->width ||
        height != s->avctx->height) {
        if (av_image_check_size(width, height, 0, s->avctx))
            return AVERROR_INVALIDDATA;

        vp8_decode_flush_impl(s->avctx, 1, 0);

        avcodec_set_dimensions(s->avctx, width, height);
    }

    s->mb_width  = (s->avctx->coded_width +15) / 16;
    s->mb_height = (s->avctx->coded_height+15) / 16;

    s->macroblocks_base        = av_mallocz((s->mb_width+s->mb_height*2+1)*sizeof(*s->macroblocks));
    s->filter_strength         = av_mallocz(s->mb_width*sizeof(*s->filter_strength));
    s->intra4x4_pred_mode_top  = av_mallocz(s->mb_width*4);
    s->top_nnz                 = av_mallocz(s->mb_width*sizeof(*s->top_nnz));
    s->top_border              = av_mallocz((s->mb_width+1)*sizeof(*s->top_border));

    if (!s->macroblocks_base || !s->filter_strength || !s->intra4x4_pred_mode_top ||
        !s->top_nnz || !s->top_border)
        return AVERROR(ENOMEM);

    s->macroblocks        = s->macroblocks_base + 1;

    return 0;
}
