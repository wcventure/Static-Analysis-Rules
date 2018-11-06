static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    StereoWidenContext *s = ctx->priv;

    s->length = 2 * s->delay * inlink->sample_rate / 1000;
    s->buffer = av_calloc(s->length, sizeof(*s->buffer));
    if (!s->buffer)
        return AVERROR(ENOMEM);
    s->write = s->buffer;

    return 0;
}
