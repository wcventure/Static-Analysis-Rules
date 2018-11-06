static int apc_read_header(AVFormatContext *s)
{
    AVIOContext *pb = s->pb;
    AVStream *st;

    avio_rl32(pb); /
    avio_rl32(pb); /
    avio_rl32(pb); /

    st = avformat_new_stream(s, NULL);
    if (!st)
        return AVERROR(ENOMEM);

    st->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
    st->codecpar->codec_id = AV_CODEC_ID_ADPCM_IMA_APC;

    avio_rl32(pb); /
    st->codecpar->sample_rate = avio_rl32(pb);

    /
    if (ff_get_extradata(s, st->codecpar, pb, 2 * 4) < 0)
        return AVERROR(ENOMEM);

    if (avio_rl32(pb)) {
        st->codecpar->channels       = 2;
        st->codecpar->channel_layout = AV_CH_LAYOUT_STEREO;
    } else {
        st->codecpar->channels       = 1;
        st->codecpar->channel_layout = AV_CH_LAYOUT_MONO;
    }

    st->codecpar->bits_per_coded_sample = 4;
    st->codecpar->bit_rate = st->codecpar->bits_per_coded_sample * st->codecpar->channels
                          * st->codecpar->sample_rate;
    st->codecpar->block_align = 1;

    return 0;
}
