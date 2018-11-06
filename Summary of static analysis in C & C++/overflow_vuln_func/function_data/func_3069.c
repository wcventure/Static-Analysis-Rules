static int shall_we_drop(AVFormatContext *s, int index)
{
    struct dshow_ctx *ctx = s->priv_data;
    static const uint8_t dropscore[] = {62, 75, 87, 100};
    const int ndropscores = FF_ARRAY_ELEMS(dropscore);
    unsigned int buffer_fullness = (ctx->curbufsize[index]*100)/s->max_picture_buffer;

    if(dropscore[++ctx->video_frame_num%ndropscores] <= buffer_fullness) {
        av_log(s, AV_LOG_ERROR,
              "real-time buffer[%d] too full (%d%% of size: %d)! frame dropped!\n", index, buffer_fullness, s->max_picture_buffer);
        return 1;
    }

    return 0;
}
