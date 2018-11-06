static void start_frame(AVFilterLink *inlink, AVFilterBufferRef *inpicref)
{
    AVFilterBufferRef *outpicref = avfilter_ref_buffer(inpicref, ~0);
    AVFilterContext *ctx = inlink->dst;
    OverlayContext *over = ctx->priv;
    av_unused AVFilterLink *outlink = ctx->outputs[0];

    inlink->dst->outputs[0]->out_buf = outpicref;
    outpicref->pts = av_rescale_q(outpicref->pts, ctx->inputs[MAIN]->time_base,
                                  ctx->outputs[0]->time_base);

    if (!over->overpicref || over->overpicref->pts < outpicref->pts) {
        if (!over->overpicref_next)
            avfilter_request_frame(ctx->inputs[OVERLAY]);

        if (over->overpicref && over->overpicref_next &&
            over->overpicref_next->pts <= outpicref->pts) {
            avfilter_unref_buffer(over->overpicref);
            over->overpicref = over->overpicref_next;
            over->overpicref_next = NULL;
        }
    }

    av_dlog(ctx, "main_pts:%s main_pts_time:%s",
            av_ts2str(outpicref->pts), av_ts2timestr(outpicref->pts, &outlink->time_base));
    if (over->overpicref)
        av_dlog(ctx, " over_pts:%s over_pts_time:%s",
                av_ts2str(over->overpicref->pts), av_ts2timestr(over->overpicref->pts, &outlink->time_base));
    av_dlog(ctx, "\n");

    avfilter_start_frame(inlink->dst->outputs[0], outpicref);
}
