static int process_work_frame(AVFilterContext *ctx)
{
    FrameRateContext *s = ctx->priv;
    int64_t work_pts;
    int interpolate;
    int ret;

    if (!s->f1)
        return 0;
    if (!s->f0 && !s->flush)
        return 0;

    work_pts = s->start_pts + av_rescale_q(s->n, av_inv_q(s->dest_frame_rate), s->dest_time_base);

    if (work_pts >= s->pts1 && !s->flush)
        return 0;

    if (!s->f0) {
        s->work = av_frame_clone(s->f1);
    } else {
        if (work_pts >= s->pts1 + s->delta && s->flush)
            return 0;

        interpolate = av_rescale(work_pts - s->pts0, s->max, s->delta);
        ff_dlog(ctx, "process_work_frame() interpolate:%d/%d\n", interpolate, s->max);
        if (interpolate > s->interp_end) {
            s->work = av_frame_clone(s->f1);
        } else if (interpolate < s->interp_start) {
            s->work = av_frame_clone(s->f0);
        } else {
            ret = blend_frames(ctx, interpolate);
            if (ret < 0)
                return ret;
            if (ret == 0)
                s->work = av_frame_clone(interpolate > (s->max >> 1) ? s->f1 : s->f0);
        }
    }

    if (!s->work)
        return AVERROR(ENOMEM);

    s->work->pts = work_pts;
    s->n++;

    return 1;
}
