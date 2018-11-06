static void init_fps(int bf, int audio_preroll, int fps)
{
    AVStream *st;
    ctx = avformat_alloc_context();
    if (!ctx)
        exit(1);
    ctx->oformat = av_guess_format(format, NULL, NULL);
    if (!ctx->oformat)
        exit(1);
    ctx->pb = avio_alloc_context(iobuf, sizeof(iobuf), AVIO_FLAG_WRITE, NULL, NULL, io_write, NULL);
    if (!ctx->pb)
        exit(1);
    ctx->flags |= AVFMT_FLAG_BITEXACT;

    st = avformat_new_stream(ctx, NULL);
    if (!st)
        exit(1);
    st->codec->codec_type = AVMEDIA_TYPE_VIDEO;
    st->codec->codec_id = AV_CODEC_ID_H264;
    st->codec->width = 640;
    st->codec->height = 480;
    st->time_base.num = 1;
    st->time_base.den = 30;
    st->codec->extradata_size = sizeof(h264_extradata);
    st->codec->extradata = av_mallocz(st->codec->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
    if (!st->codec->extradata)
        exit(1);
    memcpy(st->codec->extradata, h264_extradata, sizeof(h264_extradata));
    st->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    video_st = st;

    st = avformat_new_stream(ctx, NULL);
    if (!st)
        exit(1);
    st->codec->codec_type = AVMEDIA_TYPE_AUDIO;
    st->codec->codec_id = AV_CODEC_ID_AAC;
    st->codec->sample_rate = 44100;
    st->codec->channels = 2;
    st->time_base.num = 1;
    st->time_base.den = 44100;
    st->codec->extradata_size = sizeof(aac_extradata);
    st->codec->extradata = av_mallocz(st->codec->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
    if (!st->codec->extradata)
        exit(1);
    memcpy(st->codec->extradata, aac_extradata, sizeof(aac_extradata));
    st->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    audio_st = st;

    if (avformat_write_header(ctx, &opts) < 0)
        exit(1);
    av_dict_free(&opts);

    frames = 0;
    gop_size = 30;
    duration = video_st->time_base.den / fps;
    audio_duration = 1024 * audio_st->time_base.den / audio_st->codec->sample_rate;
    if (audio_preroll)
        audio_preroll = 2048 * audio_st->time_base.den / audio_st->codec->sample_rate;

    bframes = bf;
    video_dts = bframes ? -duration : 0;
    audio_dts = -audio_preroll;
}
