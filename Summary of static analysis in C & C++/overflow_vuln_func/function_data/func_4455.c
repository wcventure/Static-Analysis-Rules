static int dcstr_read_header(AVFormatContext *s)
{
    unsigned codec, align;
    AVStream *st;

    st = avformat_new_stream(s, NULL);
    if (!st)
        return AVERROR(ENOMEM);

    st->codecpar->codec_type  = AVMEDIA_TYPE_AUDIO;
    st->codecpar->channels    = avio_rl32(s->pb);
    st->codecpar->sample_rate = avio_rl32(s->pb);
    codec                  = avio_rl32(s->pb);
    align                  = avio_rl32(s->pb);
    avio_skip(s->pb, 4);
    st->duration           = avio_rl32(s->pb);
    st->codecpar->channels   *= avio_rl32(s->pb);
    if (!align || align > INT_MAX / st->codecpar->channels)
        return AVERROR_INVALIDDATA;
    st->codecpar->block_align = align * st->codecpar->channels;

    switch (codec) {
    case  4: st->codecpar->codec_id = AV_CODEC_ID_ADPCM_AICA;       break;
    case 16: st->codecpar->codec_id = AV_CODEC_ID_PCM_S16LE_PLANAR; break;
    default: avpriv_request_sample(s, "codec %X", codec);
             return AVERROR_PATCHWELCOME;
    }

    avio_skip(s->pb, 0x800 - avio_tell(s->pb));
    avpriv_set_pts_info(st, 64, 1, st->codecpar->sample_rate);

    return 0;
}
