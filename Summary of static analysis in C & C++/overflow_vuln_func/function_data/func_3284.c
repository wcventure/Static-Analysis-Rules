static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    StereoWidenContext *s = ctx->priv;
    const float *src = (const float *)in->data[0];
    const float drymix = s->drymix;
    const float crossfeed = s->crossfeed;
    const float feedback = s->feedback;
    AVFrame *out;
    float *dst;
    int n;

    if (av_frame_is_writable(in)) {
        out = in;
    } else {
        out = ff_get_audio_buffer(inlink, in->nb_samples);
        if (!out) {
            av_frame_free(&in);
            return AVERROR(ENOMEM);
        }
        av_frame_copy_props(out, in);
    }
    dst = (float *)out->data[0];

    for (n = 0; n < in->nb_samples; n++, src += 2, dst += 2) {
        const float left = src[0], right = src[1];
        float *read = s->write + 2;

        if (read > s->buffer + s->length)
            read = s->buffer;

        dst[0] = drymix * left - crossfeed * right - feedback * read[1];
        dst[1] = drymix * right - crossfeed * left - feedback * read[0];

        s->write[0] = left;
        s->write[1] = right;

        if (s->write == s->buffer + s->length)
            s->write = s->buffer;
        else
            s->write += 2;
    }

    if (out != in)
        av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
