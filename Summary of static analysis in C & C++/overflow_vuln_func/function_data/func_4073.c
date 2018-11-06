static int pvf_read_header(AVFormatContext *s)
{
    char buffer[32];
    AVStream *st;
    int bps, channels, sample_rate;

    avio_skip(s->pb, 5);
    ff_get_line(s->pb, buffer, sizeof(buffer));
    if (sscanf(buffer, "%d %d %d",
               &channels,
               &sample_rate,
               &bps) != 3)
        return AVERROR_INVALIDDATA;

    if (channels <= 0 || bps <= 0 || sample_rate <= 0)
        return AVERROR_INVALIDDATA;

    st = avformat_new_stream(s, NULL);
    if (!st)
        return AVERROR(ENOMEM);

    st->codecpar->codec_type  = AVMEDIA_TYPE_AUDIO;
    st->codecpar->channels    = channels;
    st->codecpar->sample_rate = sample_rate;
    st->codecpar->codec_id    = ff_get_pcm_codec_id(bps, 0, 1, 0xFFFF);
    st->codecpar->bits_per_coded_sample = bps;
    st->codecpar->block_align = bps * st->codecpar->channels / 8;

    avpriv_set_pts_info(st, 64, 1, st->codecpar->sample_rate);

    return 0;
}
