static int parse_strk(AVFormatContext *s,
                      FourxmDemuxContext *fourxm, uint8_t *buf, int size,
                      int left)
{
    AVStream *st;
    int track;
    /
    if (size != strk_SIZE || left < size + 8)
        return AVERROR_INVALIDDATA;

    track = AV_RL32(buf + 8);
    if ((unsigned)track >= UINT_MAX / sizeof(AudioTrack) - 1) {
        av_log(s, AV_LOG_ERROR, "current_track too large\n");
        return AVERROR_INVALIDDATA;
    }

    if (track + 1 > fourxm->track_count) {
        if (av_reallocp_array(&fourxm->tracks, track + 1, sizeof(AudioTrack)))
            return AVERROR(ENOMEM);
        memset(&fourxm->tracks[fourxm->track_count], 0,
               sizeof(AudioTrack) * (track + 1 - fourxm->track_count));
        fourxm->track_count = track + 1;
    }
    fourxm->tracks[track].adpcm       = AV_RL32(buf + 12);
    fourxm->tracks[track].channels    = AV_RL32(buf + 36);
    fourxm->tracks[track].sample_rate = AV_RL32(buf + 40);
    fourxm->tracks[track].bits        = AV_RL32(buf + 44);
    fourxm->tracks[track].audio_pts   = 0;

    if (fourxm->tracks[track].channels    <= 0 ||
        fourxm->tracks[track].sample_rate <= 0 ||
        fourxm->tracks[track].bits        <= 0) {
        av_log(s, AV_LOG_ERROR, "audio header invalid\n");
        return AVERROR_INVALIDDATA;
    }
    if (!fourxm->tracks[track].adpcm && fourxm->tracks[track].bits<8) {
        av_log(s, AV_LOG_ERROR, "bits unspecified for non ADPCM\n");
        return AVERROR_INVALIDDATA;
    }

    /
    st = avformat_new_stream(s, NULL);
    if (!st)
        return AVERROR(ENOMEM);

    st->id = track;
    avpriv_set_pts_info(st, 60, 1, fourxm->tracks[track].sample_rate);

    fourxm->tracks[track].stream_index = st->index;

    st->codecpar->codec_type            = AVMEDIA_TYPE_AUDIO;
    st->codecpar->codec_tag             = 0;
    st->codecpar->channels              = fourxm->tracks[track].channels;
    st->codecpar->sample_rate           = fourxm->tracks[track].sample_rate;
    st->codecpar->bits_per_coded_sample = fourxm->tracks[track].bits;
    st->codecpar->bit_rate              = st->codecpar->channels *
                                          st->codecpar->sample_rate *
                                          st->codecpar->bits_per_coded_sample;
    st->codecpar->block_align           = st->codecpar->channels *
                                          st->codecpar->bits_per_coded_sample;

    if (fourxm->tracks[track].adpcm){
        st->codecpar->codec_id = AV_CODEC_ID_ADPCM_4XM;
    } else if (st->codecpar->bits_per_coded_sample == 8) {
        st->codecpar->codec_id = AV_CODEC_ID_PCM_U8;
    } else
        st->codecpar->codec_id = AV_CODEC_ID_PCM_S16LE;

    return 0;
}
