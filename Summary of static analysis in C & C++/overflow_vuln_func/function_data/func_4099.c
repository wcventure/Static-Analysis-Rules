inline static int push_frame(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AVFilterLink *inlink = ctx->inputs[0];
    ShowWavesContext *showwaves = outlink->src->priv;
    int nb_channels = inlink->channels;
    int ret, i;

    if ((ret = ff_filter_frame(outlink, showwaves->outpicref)) >= 0)
        showwaves->req_fullfilled = 1;
    showwaves->outpicref = NULL;
    showwaves->buf_idx = 0;
    for (i = 0; i <= nb_channels; i++)
        showwaves->buf_idy[i] = 0;
    return ret;
}
