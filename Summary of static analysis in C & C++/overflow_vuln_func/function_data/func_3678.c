static int au_read_header(AVFormatContext *s)
{
    int size, bps, data_size = 0;
    unsigned int tag;
    AVIOContext *pb = s->pb;
    unsigned int id, channels, rate;
    enum CodecID codec;
    AVStream *st;

    /
    tag = avio_rl32(pb);
    if (tag != MKTAG('.', 's', 'n', 'd'))
        return -1;
    size = avio_rb32(pb); /
    data_size = avio_rb32(pb); /

    if (data_size < 0 && data_size != AU_UNKNOWN_SIZE) {
        av_log(s, AV_LOG_ERROR, "Invalid negative data size '%d' found\n", data_size);
        return AVERROR_INVALIDDATA;
    }

    id = avio_rb32(pb);
    rate = avio_rb32(pb);
    channels = avio_rb32(pb);

    codec = ff_codec_get_id(codec_au_tags, id);

    if (!(bps = av_get_bits_per_sample(codec))) {
        av_log_ask_for_sample(s, "could not determine bits per sample\n");
        return AVERROR_INVALIDDATA;
    }

    if (channels <= 0) {
        av_log(s, AV_LOG_ERROR, "Invalid number of channels %d\n", channels);
        return AVERROR_INVALIDDATA;
    }

    if (size >= 24) {
        /
        avio_skip(pb, size - 24);
    }

    /
    st = avformat_new_stream(s, NULL);
    if (!st)
        return -1;
    st->codec->codec_type = AVMEDIA_TYPE_AUDIO;
    st->codec->codec_tag = id;
    st->codec->codec_id = codec;
    st->codec->channels = channels;
    st->codec->sample_rate = rate;
    if (data_size != AU_UNKNOWN_SIZE)
    st->duration = (((int64_t)data_size)<<3) / (st->codec->channels * bps);
    avpriv_set_pts_info(st, 64, 1, rate);
    return 0;
}
