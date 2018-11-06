static int rtsp_read_header(AVFormatContext *s,
                            AVFormatParameters *ap)
{
    RTSPState *rt = s->priv_data;
    int ret;

    ret = ff_rtsp_connect(s);
    if (ret)
        return ret;

    rt->real_setup_cache = av_mallocz(2 * s->nb_streams * sizeof(*rt->real_setup_cache));
    if (!rt->real_setup_cache)
        return AVERROR(ENOMEM);
    rt->real_setup = rt->real_setup_cache + s->nb_streams * sizeof(*rt->real_setup);

    if (ap->initial_pause) {
         /
    } else {
         if (rtsp_read_play(s) < 0) {
            ff_rtsp_close_streams(s);
            ff_rtsp_close_connections(s);
            return AVERROR_INVALIDDATA;
        }
    }

    return 0;
}
