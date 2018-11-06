static void start_frame_overlay(AVFilterLink *inlink, AVFilterBufferRef *inpicref)
{
    AVFilterContext *ctx = inlink->dst;
    OverlayContext *over = ctx->priv;

    inpicref->pts = av_rescale_q(inpicref->pts, ctx->inputs[OVERLAY]->time_base,
                                 ctx->outputs[0]->time_base);

    if (!over->overpicref) over->overpicref      = inpicref;
    else                   over->overpicref_next = inpicref;
}
