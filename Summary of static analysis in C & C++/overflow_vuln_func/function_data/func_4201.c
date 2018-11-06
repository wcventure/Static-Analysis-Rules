static av_cold void uninit(AVFilterContext *ctx)
{
    OverlayContext *over = ctx->priv;

    av_freep(&over->x_expr);
    av_freep(&over->y_expr);

    if (over->overpicref)
        avfilter_unref_buffer(over->overpicref);
    if (over->overpicref_next)
        avfilter_unref_buffer(over->overpicref_next);
}
