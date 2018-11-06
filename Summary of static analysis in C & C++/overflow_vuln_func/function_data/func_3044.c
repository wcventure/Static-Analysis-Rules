static int xvag_read_header(AVFormatContext *s)
{
    unsigned offset, big_endian, codec;
    AVStream *st;

    avio_skip(s->pb, 4);

    st = avformat_new_stream(s, NULL);
    if (!st)
        return AVERROR(ENOMEM);

    st->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;

    offset     = avio_rl32(s->pb);
    big_endian = offset > av_bswap32(offset);
    if (big_endian) {
        offset                 = av_bswap32(offset);
        avio_skip(s->pb, 28);
        codec                  = avio_rb32(s->pb);
        st->codecpar->channels = avio_rb32(s->pb);
        avio_skip(s->pb, 4);
        st->duration           = avio_rb32(s->pb);
        avio_skip(s->pb, 8);
        st->codecpar->sample_rate = avio_rb32(s->pb);
    } else {
        avio_skip(s->pb, 28);
        codec                  = avio_rl32(s->pb);
        st->codecpar->channels = avio_rl32(s->pb);
        avio_skip(s->pb, 4);
        st->duration           = avio_rl32(s->pb);
        avio_skip(s->pb, 8);
        st->codecpar->sample_rate = avio_rl32(s->pb);
    }

    if (st->codecpar->sample_rate <= 0)
        return AVERROR_INVALIDDATA;
    if (st->codecpar->channels <= 0)
        return AVERROR_INVALIDDATA;

    switch (codec) {
    case 0x1c:
        st->codecpar->codec_id    = AV_CODEC_ID_ADPCM_PSX;
        st->codecpar->block_align = 16 * st->codecpar->channels;
        break;
    default:
        avpriv_request_sample(s, "codec %X", codec);
        return AVERROR_PATCHWELCOME;
    };

    avio_skip(s->pb, offset - avio_tell(s->pb));

    if (avio_rb16(s->pb) == 0xFFFB) {
        st->codecpar->codec_id    = AV_CODEC_ID_MP3;
        st->codecpar->block_align = 0x1000;
        st->need_parsing       = AVSTREAM_PARSE_FULL_RAW;
    }

    avio_skip(s->pb, -2);
    avpriv_set_pts_info(st, 64, 1, st->codecpar->sample_rate);

    return 0;
}
