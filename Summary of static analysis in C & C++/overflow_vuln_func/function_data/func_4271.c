static int read_header(AVFormatContext *s)
{
    AVStream *st = avformat_new_stream(s, NULL);
    if (!st)
        return AVERROR(ENOMEM);

    st->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
    st->codecpar->codec_id = AV_CODEC_ID_ADPCM_MS;

    avio_rl32(s->pb);
    avio_rl32(s->pb);
    st->codecpar->sample_rate = avio_rl32(s->pb);
    st->codecpar->channels    = avio_rl32(s->pb);
    s->internal->data_offset = avio_rl32(s->pb);
    avio_r8(s->pb);
    st->codecpar->block_align = st->codecpar->channels * avio_rl32(s->pb);

    avio_seek(s->pb, s->internal->data_offset, SEEK_SET);

    return 0;
}
