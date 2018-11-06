static int au_read_header(AVFormatContext *s)
{
    int size;
    unsigned int tag;
    AVIOContext *pb = s->pb;
    unsigned int id, channels, rate;
    int bps;
    enum AVCodecID codec;
    AVStream *st;

    /
    tag = avio_rl32(pb);
    if (tag != MKTAG('.', 's', 'n', 'd'))
        return -1;
    size = avio_rb32(pb); /
    avio_rb32(pb); /

    id = avio_rb32(pb);
    rate = avio_rb32(pb);
    channels = avio_rb32(pb);

    codec = ff_codec_get_id(codec_au_tags, id);

    if (codec == AV_CODEC_ID_NONE) {
        av_log_ask_for_sample(s, "unknown or unsupported codec tag: %d\n", id);
        return AVERROR_PATCHWELCOME;
    }

    bps = av_get_bits_per_sample(codec);
    if (!bps) {
        av_log_ask_for_sample(s, "could not determine bits per sample\n");
        return AVERROR_PATCHWELCOME;
    }

    if (channels == 0 || channels > 64) {
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
    st->codec->bit_rate    = channels * rate * bps;
    st->codec->block_align = channels * bps >> 3;
    avpriv_set_pts_info(st, 64, 1, rate);
    return 0;
}
