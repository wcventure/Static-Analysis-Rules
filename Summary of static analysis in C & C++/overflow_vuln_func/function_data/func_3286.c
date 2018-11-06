static int check_timecode(void *log_ctx, AVTimecode *tc)
{
    if (tc->fps <= 0) {
        av_log(log_ctx, AV_LOG_ERROR, "Timecode frame rate must be specified\n");
        return AVERROR(EINVAL);
    }
    if ((tc->flags & AV_TIMECODE_FLAG_DROPFRAME) && tc->fps != 30 && tc->fps != 60) {
        av_log(log_ctx, AV_LOG_ERROR, "Drop frame is only allowed with 30000/1001 or 60000/1001 FPS\n");
        return AVERROR(EINVAL);
    }
    if (check_fps(tc->fps) < 0) {
        av_log(log_ctx, AV_LOG_WARNING, "Using non-standard frame rate %d/%d\n",
               tc->rate.num, tc->rate.den);
    }
    return 0;
}
