static int rsd_read_header(AVFormatContext *s)
{
    AVIOContext *pb = s->pb;
    int i, ret, version, start = 0x800;
    AVCodecParameters *par;
    AVStream *st = avformat_new_stream(s, NULL);

    if (!st)
        return AVERROR(ENOMEM);

    avio_skip(pb, 3); // "RSD"
    version = avio_r8(pb) - '0';

    par = st->codecpar;
    par->codec_type = AVMEDIA_TYPE_AUDIO;
    par->codec_tag  = avio_rl32(pb);
    par->codec_id   = ff_codec_get_id(rsd_tags, par->codec_tag);
    if (!par->codec_id) {
        char tag_buf[32];

        av_get_codec_tag_string(tag_buf, sizeof(tag_buf), par->codec_tag);
        for (i=0; i < FF_ARRAY_ELEMS(rsd_unsupported_tags); i++) {
            if (par->codec_tag == rsd_unsupported_tags[i]) {
                avpriv_request_sample(s, "Codec tag: %s", tag_buf);
                return AVERROR_PATCHWELCOME;
            }
        }
        av_log(s, AV_LOG_ERROR, "Unknown codec tag: %s\n", tag_buf);
        return AVERROR_INVALIDDATA;
    }

    par->channels = avio_rl32(pb);
    if (!par->channels)
        return AVERROR_INVALIDDATA;

    avio_skip(pb, 4); // Bit depth
    par->sample_rate = avio_rl32(pb);
    if (!par->sample_rate)
        return AVERROR_INVALIDDATA;

    avio_skip(pb, 4); // Unknown

    switch (par->codec_id) {
    case AV_CODEC_ID_XMA2:
        par->block_align = 2048;
        ff_alloc_extradata(par, 34);
        if (!par->extradata)
            return AVERROR(ENOMEM);
        memset(par->extradata, 0, 34);
        break;
    case AV_CODEC_ID_ADPCM_PSX:
        par->block_align = 16 * par->channels;
        if (pb->seekable)
            st->duration = av_get_audio_frame_duration2(par, avio_size(pb) - start);
        break;
    case AV_CODEC_ID_ADPCM_IMA_RAD:
        par->block_align = 20 * par->channels;
        if (pb->seekable)
            st->duration = av_get_audio_frame_duration2(par, avio_size(pb) - start);
        break;
    case AV_CODEC_ID_ADPCM_IMA_WAV:
        if (version == 2)
            start = avio_rl32(pb);

        par->bits_per_coded_sample = 4;
        par->block_align = 36 * par->channels;
        if (pb->seekable)
            st->duration = av_get_audio_frame_duration2(par, avio_size(pb) - start);
        break;
    case AV_CODEC_ID_ADPCM_THP_LE:
        /

        start = avio_rl32(pb);

        if ((ret = ff_get_extradata(s, par, s->pb, 32)) < 0)
            return ret;
        if (pb->seekable)
            st->duration = av_get_audio_frame_duration2(par, avio_size(pb) - start);
        break;
    case AV_CODEC_ID_ADPCM_THP:
        par->block_align = 8 * par->channels;
        avio_skip(s->pb, 0x1A4 - avio_tell(s->pb));

        if ((ret = ff_alloc_extradata(st->codecpar, 32 * par->channels)) < 0)
            return ret;

        for (i = 0; i < par->channels; i++) {
            avio_read(s->pb, st->codecpar->extradata + 32 * i, 32);
            avio_skip(s->pb, 8);
        }
        if (pb->seekable)
            st->duration = (avio_size(pb) - start) / (8 * par->channels) * 14;
        break;
    case AV_CODEC_ID_PCM_S16LE:
    case AV_CODEC_ID_PCM_S16BE:
        if (version != 4)
            start = avio_rl32(pb);

        if (pb->seekable)
            st->duration = (avio_size(pb) - start) / 2 / par->channels;
        break;
    }

    avio_skip(pb, start - avio_tell(pb));
    if (par->codec_id == AV_CODEC_ID_XMA2) {
        avio_skip(pb, avio_rb32(pb) + avio_rb32(pb));
        st->duration = avio_rb32(pb);
    }

    avpriv_set_pts_info(st, 64, 1, par->sample_rate);

    return 0;
}
