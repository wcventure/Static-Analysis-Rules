static int filter_frame(AVFilterLink *inlink, AVFrame *insamples)
{
    AVFilterContext *ctx = inlink->dst;
    AMergeContext *am = ctx->priv;
    AVFilterLink *const outlink = ctx->outputs[0];
    int input_number;
    int nb_samples, ns, i;
    AVFrame *outbuf, *inbuf[SWR_CH_MAX];
    uint8_t *ins[SWR_CH_MAX], *outs;

    for (input_number = 0; input_number < am->nb_inputs; input_number++)
        if (inlink == ctx->inputs[input_number])
            break;
    av_assert1(input_number < am->nb_inputs);
    if (ff_bufqueue_is_full(&am->in[input_number].queue)) {
        av_log(ctx, AV_LOG_ERROR, "Buffer queue overflow\n");
        av_frame_free(&insamples);
        return AVERROR(ENOMEM);
    }
    ff_bufqueue_add(ctx, &am->in[input_number].queue, av_frame_clone(insamples));
    am->in[input_number].nb_samples += insamples->nb_samples;
    av_frame_free(&insamples);
    nb_samples = am->in[0].nb_samples;
    for (i = 1; i < am->nb_inputs; i++)
        nb_samples = FFMIN(nb_samples, am->in[i].nb_samples);
    if (!nb_samples)
        return 0;

    outbuf = ff_get_audio_buffer(ctx->outputs[0], nb_samples);
    if (!outbuf)
        return AVERROR(ENOMEM);
    outs = outbuf->data[0];
    for (i = 0; i < am->nb_inputs; i++) {
        inbuf[i] = ff_bufqueue_peek(&am->in[i].queue, 0);
        ins[i] = inbuf[i]->data[0] +
                 am->in[i].pos * am->in[i].nb_ch * am->bps;
    }
    av_frame_copy_props(outbuf, inbuf[0]);
    outbuf->pts = inbuf[0]->pts == AV_NOPTS_VALUE ? AV_NOPTS_VALUE :
                  inbuf[0]->pts +
                  av_rescale_q(am->in[0].pos,
                               (AVRational){ 1, ctx->inputs[0]->sample_rate },
                               ctx->outputs[0]->time_base);

    outbuf->nb_samples     = nb_samples;
    outbuf->channel_layout = outlink->channel_layout;
    av_frame_set_channels(outbuf, outlink->channels);

    while (nb_samples) {
        ns = nb_samples;
        for (i = 0; i < am->nb_inputs; i++)
            ns = FFMIN(ns, inbuf[i]->nb_samples - am->in[i].pos);
        /
        switch (am->bps) {
            case 1:
                copy_samples(am->nb_inputs, am->in, am->route, ins, &outs, ns, 1);
                break;
            case 2:
                copy_samples(am->nb_inputs, am->in, am->route, ins, &outs, ns, 2);
                break;
            case 4:
                copy_samples(am->nb_inputs, am->in, am->route, ins, &outs, ns, 4);
                break;
            default:
                copy_samples(am->nb_inputs, am->in, am->route, ins, &outs, ns, am->bps);
                break;
        }

        nb_samples -= ns;
        for (i = 0; i < am->nb_inputs; i++) {
            am->in[i].nb_samples -= ns;
            am->in[i].pos += ns;
            if (am->in[i].pos == inbuf[i]->nb_samples) {
                am->in[i].pos = 0;
                av_frame_free(&inbuf[i]);
                ff_bufqueue_get(&am->in[i].queue);
                inbuf[i] = ff_bufqueue_peek(&am->in[i].queue, 0);
                ins[i] = inbuf[i] ? inbuf[i]->data[0] : NULL;
            }
        }
    }
    return ff_filter_frame(ctx->outputs[0], outbuf);
}
