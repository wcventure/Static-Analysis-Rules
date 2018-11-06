static int hls_write_header(AVFormatContext *s)
{
    HLSContext *hls = s->priv_data;
    int ret, i;
    char *p;
    const char *pattern = "%d.ts";
    int basename_size = strlen(s->filename) + strlen(pattern);

    hls->number      = 0;

    hls->recording_time = hls->time * 1000000;
    hls->start_pts      = AV_NOPTS_VALUE;

    for (i = 0; i < s->nb_streams; i++)
        hls->has_video +=
            s->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO;

    if (hls->has_video > 1)
        av_log(s, AV_LOG_WARNING,
               "More than a single video stream present, "
               "expect issues decoding it.\n");

    hls->oformat = av_guess_format("mpegts", NULL, NULL);

    if (!hls->oformat) {
        ret = AVERROR_MUXER_NOT_FOUND;
        goto fail;
    }

    hls->basename = av_malloc(basename_size);

    if (!hls->basename) {
        ret = AVERROR(ENOMEM);
        goto fail;
    }

    strcpy(hls->basename, s->filename);

    p = strrchr(hls->basename, '.');

    if (p)
        *p = '\0';

    av_strlcat(hls->basename, "%d.ts", basename_size);

    if ((ret = hls_mux_init(s)) < 0)
        goto fail;

    if ((ret = hls_start(s)) < 0)
        goto fail;

    if ((ret = avformat_write_header(hls->avf, NULL)) < 0)
        return ret;


fail:
    if (ret) {
        av_free(hls->basename);
        if (hls->avf)
            avformat_free_context(hls->avf);
    }
    return ret;
}
