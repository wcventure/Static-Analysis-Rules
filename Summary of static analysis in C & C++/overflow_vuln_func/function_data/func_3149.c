static int ea_read_header(AVFormatContext *s)
{
    EaDemuxContext *ea = s->priv_data;
    AVStream *st;

    if (process_ea_header(s)<=0)
        return AVERROR(EIO);

    if (init_video_stream(s, &ea->video) || init_video_stream(s, &ea->alpha))
        return AVERROR(ENOMEM);

    if (ea->audio_codec) {
        if (ea->num_channels <= 0 || ea->num_channels > 2) {
            av_log(s, AV_LOG_WARNING,
                   "Unsupported number of channels: %d\n", ea->num_channels);
            ea->audio_codec = 0;
            return 1;
        }
        if (ea->sample_rate <= 0) {
            av_log(s, AV_LOG_ERROR,
                   "Unsupported sample rate: %d\n", ea->sample_rate);
            ea->audio_codec = 0;
            return 1;
        }
        if (ea->bytes <= 0) {
            av_log(s, AV_LOG_ERROR,
                   "Invalid number of bytes per sample: %d\n", ea->bytes);
            ea->audio_codec = AV_CODEC_ID_NONE;
            return 1;
        }

        /
        st = avformat_new_stream(s, NULL);
        if (!st)
            return AVERROR(ENOMEM);
        avpriv_set_pts_info(st, 33, 1, ea->sample_rate);
        st->codecpar->codec_type            = AVMEDIA_TYPE_AUDIO;
        st->codecpar->codec_id              = ea->audio_codec;
        st->codecpar->codec_tag             = 0;   /
        st->codecpar->channels              = ea->num_channels;
        st->codecpar->sample_rate           = ea->sample_rate;
        st->codecpar->bits_per_coded_sample = ea->bytes * 8;
        st->codecpar->bit_rate              = st->codecpar->channels *
                                              st->codecpar->sample_rate *
                                              st->codecpar->bits_per_coded_sample / 4;
        st->codecpar->block_align           = st->codecpar->channels *
                                              st->codecpar->bits_per_coded_sample;
        ea->audio_stream_index           = st->index;
        st->start_time                   = 0;
    }

    return 1;
}
