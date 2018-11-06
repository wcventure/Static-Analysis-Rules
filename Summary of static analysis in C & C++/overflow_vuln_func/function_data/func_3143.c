static int oma_read_header(AVFormatContext *s)
{
    int     ret, framesize, jsflag, samplerate;
    uint32_t codec_params, channel_id;
    int16_t eid;
    uint8_t buf[EA3_HEADER_SIZE];
    uint8_t *edata;
    AVStream *st;
    ID3v2ExtraMeta *extra_meta = NULL;
    OMAContext *oc = s->priv_data;

    ff_id3v2_read(s, ID3v2_EA3_MAGIC, &extra_meta, 0);
    ret = avio_read(s->pb, buf, EA3_HEADER_SIZE);
    if (ret < EA3_HEADER_SIZE)
        return -1;

    if (memcmp(buf, ((const uint8_t[]){'E', 'A', '3'}), 3) ||
        buf[4] != 0 || buf[5] != EA3_HEADER_SIZE) {
        av_log(s, AV_LOG_ERROR, "Couldn't find the EA3 header !\n");
        return AVERROR_INVALIDDATA;
    }

    oc->content_start = avio_tell(s->pb);

    /
    eid = AV_RB16(&buf[6]);
    if (eid != -1 && eid != -128 && decrypt_init(s, extra_meta, buf) < 0) {
        ff_id3v2_free_extra_meta(&extra_meta);
        return -1;
    }

    ff_id3v2_free_extra_meta(&extra_meta);

    codec_params = AV_RB24(&buf[33]);

    st = avformat_new_stream(s, NULL);
    if (!st)
        return AVERROR(ENOMEM);

    st->start_time = 0;
    st->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
    st->codecpar->codec_tag  = buf[32];
    st->codecpar->codec_id   = ff_codec_get_id(ff_oma_codec_tags,
                                               st->codecpar->codec_tag);

    switch (buf[32]) {
    case OMA_CODECID_ATRAC3:
        samplerate = ff_oma_srate_tab[(codec_params >> 13) & 7] * 100;
        if (!samplerate) {
            av_log(s, AV_LOG_ERROR, "Unsupported sample rate\n");
            return AVERROR_INVALIDDATA;
        }
        if (samplerate != 44100)
            avpriv_request_sample(s, "Sample rate %d", samplerate);

        framesize = (codec_params & 0x3FF) * 8;

        /
        jsflag = (codec_params >> 17) & 1;

        st->codecpar->channels    = 2;
        st->codecpar->channel_layout = AV_CH_LAYOUT_STEREO;
        st->codecpar->sample_rate = samplerate;
        st->codecpar->bit_rate    = st->codecpar->sample_rate * framesize * 8 / 1024;

        /
        if (ff_alloc_extradata(st->codecpar, 14))
            return AVERROR(ENOMEM);

        edata = st->codecpar->extradata;
        AV_WL16(&edata[0],  1);             // always 1
        AV_WL32(&edata[2],  samplerate);    // samples rate
        AV_WL16(&edata[6],  jsflag);        // coding mode
        AV_WL16(&edata[8],  jsflag);        // coding mode
        AV_WL16(&edata[10], 1);             // always 1
        // AV_WL16(&edata[12], 0);          // always 0

        avpriv_set_pts_info(st, 64, 1, st->codecpar->sample_rate);
        break;
    case OMA_CODECID_ATRAC3P:
        channel_id = (codec_params >> 10) & 7;
        if (!channel_id) {
            av_log(s, AV_LOG_ERROR,
                   "Invalid ATRAC-X channel id: %"PRIu32"\n", channel_id);
            return AVERROR_INVALIDDATA;
        }
        st->codecpar->channel_layout = ff_oma_chid_to_native_layout[channel_id - 1];
        st->codecpar->channels       = ff_oma_chid_to_num_channels[channel_id - 1];
        framesize = ((codec_params & 0x3FF) * 8) + 8;
        samplerate = ff_oma_srate_tab[(codec_params >> 13) & 7] * 100;
        if (!samplerate) {
            av_log(s, AV_LOG_ERROR, "Unsupported sample rate\n");
            return AVERROR_INVALIDDATA;
        }
        st->codecpar->sample_rate = samplerate;
        st->codecpar->bit_rate    = samplerate * framesize * 8 / 2048;
        avpriv_set_pts_info(st, 64, 1, samplerate);
        break;
    case OMA_CODECID_MP3:
        st->need_parsing = AVSTREAM_PARSE_FULL_RAW;
        framesize = 1024;
        break;
    case OMA_CODECID_LPCM:
        /
        st->codecpar->channels = 2;
        st->codecpar->channel_layout = AV_CH_LAYOUT_STEREO;
        st->codecpar->sample_rate = 44100;
        framesize = 1024;
        /
        st->codecpar->bit_rate = st->codecpar->sample_rate * 32;
        st->codecpar->bits_per_coded_sample =
            av_get_bits_per_sample(st->codecpar->codec_id);
        avpriv_set_pts_info(st, 64, 1, st->codecpar->sample_rate);
        break;
    default:
        av_log(s, AV_LOG_ERROR, "Unsupported codec %d!\n", buf[32]);
        return AVERROR(ENOSYS);
    }

    st->codecpar->block_align = framesize;

    return 0;
}
