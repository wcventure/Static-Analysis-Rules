static int wsd_read_header(AVFormatContext *s)
{
    AVIOContext *pb = s->pb;
    AVStream *st;
    int version;
    uint32_t text_offset, data_offset, channel_assign;
    char playback_time[AV_TIMECODE_STR_SIZE];

    st = avformat_new_stream(s, NULL);
    if (!st)
        return AVERROR(ENOMEM);

    avio_skip(pb, 8);
    version = avio_r8(pb);
    av_log(s, AV_LOG_DEBUG, "version: %i.%i\n", version >> 4, version & 0xF);
    avio_skip(pb, 11);

    if (version < 0x10) {
        text_offset = 0x80;
        data_offset = 0x800;
        avio_skip(pb, 8);
    } else {
        text_offset = avio_rb32(pb);
        data_offset = avio_rb32(pb);
    }

    avio_skip(pb, 4);
    av_timecode_make_smpte_tc_string(playback_time, avio_rb32(pb), 0);
    av_dict_set(&s->metadata, "playback_time", playback_time, 0);

    st->codecpar->codec_type  = AVMEDIA_TYPE_AUDIO;
    st->codecpar->codec_id    = s->iformat->raw_codec_id;
    st->codecpar->sample_rate = avio_rb32(pb) / 8;
    avio_skip(pb, 4);
    st->codecpar->channels    = avio_r8(pb) & 0xF;
    st->codecpar->bit_rate    = st->codecpar->channels * st->codecpar->sample_rate * 8LL;
    if (!st->codecpar->channels)
        return AVERROR_INVALIDDATA;

    avio_skip(pb, 3);
    channel_assign         = avio_rb32(pb);
    if (!(channel_assign & 1)) {
        int i;
        for (i = 1; i < 32; i++)
            if (channel_assign & (1 << i))
                st->codecpar->channel_layout |= wsd_to_av_channel_layoyt(s, i);
    }

    avio_skip(pb, 16);
    if (avio_rb32(pb))
       avpriv_request_sample(s, "emphasis");

    if (avio_seek(pb, text_offset, SEEK_SET) >= 0) {
        get_metadata(s, "title",       128);
        get_metadata(s, "composer",    128);
        get_metadata(s, "song_writer", 128);
        get_metadata(s, "artist",      128);
        get_metadata(s, "album",       128);
        get_metadata(s, "genre",        32);
        get_metadata(s, "date",         32);
        get_metadata(s, "location",     32);
        get_metadata(s, "comment",     512);
        get_metadata(s, "user",        512);
    }

    return avio_seek(pb, data_offset, SEEK_SET);
}
