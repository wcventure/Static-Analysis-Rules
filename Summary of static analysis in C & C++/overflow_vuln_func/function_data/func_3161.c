static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    FieldHintContext *s = ctx->priv;
    AVFrame *out, *top, *bottom;
    char buf[1024] = { 0 };
    int64_t tf, bf;
    char hint = '=';
    int p;

    av_frame_free(&s->frame[0]);
    s->frame[0] = s->frame[1];
    s->frame[1] = s->frame[2];
    s->frame[2] = in;
    if (!s->frame[1])
        return 0;
    else if (!s->frame[0]) {
        s->frame[0] = av_frame_clone(s->frame[1]);
        if (!s->frame[0])
            return AVERROR(ENOMEM);
    }

    while (1) {
        if (fgets(buf, sizeof(buf)-1, s->hint)) {
            s->line++;
            if (buf[0] == '#' || buf[0] == ';') {
                continue;
            } else if (sscanf(buf, "%"PRId64",%"PRId64" %c", &tf, &bf, &hint) == 3) {
                ;
            } else if (sscanf(buf, "%"PRId64",%"PRId64"", &tf, &bf) == 2) {
                ;
            } else {
                av_log(ctx, AV_LOG_ERROR, "Invalid entry at line %"PRId64".\n", s->line);
                return AVERROR_INVALIDDATA;
            }
            switch (s->mode) {
            case 0:
                if (tf > outlink->frame_count + 1 || tf < FFMAX(0, outlink->frame_count - 1) ||
                    bf > outlink->frame_count + 1 || bf < FFMAX(0, outlink->frame_count - 1)) {
                    av_log(ctx, AV_LOG_ERROR, "Out of range frames %"PRId64" and/or %"PRId64" on line %"PRId64" for %"PRId64". input frame.\n", tf, bf, s->line, inlink->frame_count);
                    return AVERROR_INVALIDDATA;
                }
                break;
            case 1:
                if (tf > 1 || tf < -1 ||
                    bf > 1 || bf < -1) {
                    av_log(ctx, AV_LOG_ERROR, "Out of range %"PRId64" and/or %"PRId64" on line %"PRId64" for %"PRId64". input frame.\n", tf, bf, s->line, inlink->frame_count);
                    return AVERROR_INVALIDDATA;
                }
            };
            break;
        } else {
            av_log(ctx, AV_LOG_ERROR, "Missing entry for %"PRId64". input frame.\n", inlink->frame_count);
            return AVERROR_INVALIDDATA;
        }
    }

    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out)
        return AVERROR(ENOMEM);
    av_frame_copy_props(out, s->frame[1]);

    switch (s->mode) {
    case 0:
        top    = s->frame[1 + tf - outlink->frame_count];
        bottom = s->frame[1 + bf - outlink->frame_count];
        break;
    case 1:
        top    = s->frame[1 + tf];
        bottom = s->frame[1 + bf];
        break;
    default:
        av_assert0(0);
    }

    switch (hint) {
    case '+':
        out->interlaced_frame = 1;
        break;
    case '-':
        out->interlaced_frame = 0;
        break;
    case '=':
        break;
    default:
        av_log(ctx, AV_LOG_ERROR, "Invalid hint: %c.\n", hint);
        return AVERROR(EINVAL);
    }

    for (p = 0; p < s->nb_planes; p++) {
        av_image_copy_plane(out->data[p],
                            out->linesize[p] * 2,
                            top->data[p],
                            top->linesize[p] * 2,
                            s->planewidth[p],
                            (s->planeheight[p] + 1) / 2);
        av_image_copy_plane(out->data[p] + out->linesize[p],
                            out->linesize[p] * 2,
                            bottom->data[p] + bottom->linesize[p],
                            bottom->linesize[p] * 2,
                            s->planewidth[p],
                            (s->planeheight[p] + 1) / 2);
    }

    return ff_filter_frame(outlink, out);
}
