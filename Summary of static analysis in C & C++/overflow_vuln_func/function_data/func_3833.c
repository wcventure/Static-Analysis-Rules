static int msf_read_header(AVFormatContext *s)
{
    unsigned codec, align, size;
    AVStream *st;

    avio_skip(s->pb, 4);

    st = avformat_new_stream(s, NULL);
    if (!st)
        return AVERROR(ENOMEM);

    st->codec->codec_type  = AVMEDIA_TYPE_AUDIO;
    codec                  = avio_rb32(s->pb);
    st->codec->channels    = avio_rb32(s->pb);
    if (st->codec->channels <= 0)
        return AVERROR_INVALIDDATA;
    size = avio_rb32(s->pb);
    st->codec->sample_rate = avio_rb32(s->pb);
    if (st->codec->sample_rate <= 0)
        return AVERROR_INVALIDDATA;
    align = avio_rb32(s->pb) ;
    if (align > INT_MAX / st->codec->channels)
        return AVERROR_INVALIDDATA;
    st->codec->block_align = align;
    switch (codec) {
    case 0: st->codec->codec_id = AV_CODEC_ID_PCM_S16BE; break;
    case 3: st->codec->block_align = 16 * st->codec->channels;
            st->codec->codec_id = AV_CODEC_ID_ADPCM_PSX; break;
    case 7: st->need_parsing = AVSTREAM_PARSE_FULL_RAW;
            st->codec->codec_id = AV_CODEC_ID_MP3;       break;
    default:
            avpriv_request_sample(s, "Codec %d", codec);
            return AVERROR_PATCHWELCOME;
    }
    st->duration = av_get_audio_frame_duration(st->codec, size);
    avio_skip(s->pb, 0x40 - avio_tell(s->pb));
    avpriv_set_pts_info(st, 64, 1, st->codec->sample_rate);

    return 0;
}
