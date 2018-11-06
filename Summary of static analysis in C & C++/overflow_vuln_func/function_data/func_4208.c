static int poll_frame(AVFilterLink *link)
{
    AVFilterContext   *s = link->src;
    OverlayContext *over = s->priv;
    int ret = avfilter_poll_frame(s->inputs[OVERLAY]);

    if (ret == AVERROR_EOF)
        ret = !!over->overpicref;

    return ret && avfilter_poll_frame(s->inputs[MAIN]);
}
