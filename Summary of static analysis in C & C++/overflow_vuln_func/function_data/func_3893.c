static int blend_frames(AVFilterContext *ctx, int interpolate)
{
    FrameRateContext *s = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    double interpolate_scene_score = 0;

    if ((s->flags & FRAMERATE_FLAG_SCD)) {
        if (s->score >= 0.0)
            interpolate_scene_score = s->score;
        else
            interpolate_scene_score = s->score = get_scene_score(ctx, s->f0, s->f1);
        ff_dlog(ctx, "blend_frames() interpolate scene score:%f\n", interpolate_scene_score);
    }
    // decide if the shot-change detection allows us to blend two frames
    if (interpolate_scene_score < s->scene_score) {
        ThreadData td;
        td.copy_src1 = s->f0;
        td.copy_src2 = s->f1;
        td.src2_factor = interpolate;
        td.src1_factor = s->max - td.src2_factor;

        // get work-space for output frame
        s->work = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!s->work)
            return AVERROR(ENOMEM);

        av_frame_copy_props(s->work, s->f0);

        ff_dlog(ctx, "blend_frames() INTERPOLATE to create work frame\n");
        ctx->internal->execute(ctx, filter_slice, &td, NULL, FFMIN(outlink->h, ff_filter_get_nb_threads(ctx)));
        return 1;
    }
    return 0;
}
