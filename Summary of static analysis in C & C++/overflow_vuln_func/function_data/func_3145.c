static int get_aiff_header(AVFormatContext *s, int size,
                                    unsigned version)
{
    AVIOContext *pb        = s->pb;
    AVCodecParameters *par = s->streams[0]->codecpar;
    AIFFInputContext *aiff = s->priv_data;
    int exp;
    uint64_t val;
    int sample_rate;
    unsigned int num_frames;

    if (size & 1)
        size++;
    par->codec_type = AVMEDIA_TYPE_AUDIO;
    par->channels = avio_rb16(pb);
    num_frames = avio_rb32(pb);
    par->bits_per_coded_sample = avio_rb16(pb);

    exp = avio_rb16(pb) - 16383 - 63;
    val = avio_rb64(pb);
    if (exp <-63 || exp >63) {
        av_log(s, AV_LOG_ERROR, "exp %d is out of range\n", exp);
        return AVERROR_INVALIDDATA;
    }
    if (exp >= 0)
        sample_rate = val << exp;
    else
        sample_rate = (val + (1ULL<<(-exp-1))) >> -exp;
    par->sample_rate = sample_rate;
    size -= 18;

    /
    if (size < 4) {
        version = AIFF;
    } else if (version == AIFF_C_VERSION1) {
        par->codec_tag = avio_rl32(pb);
        par->codec_id  = ff_codec_get_id(ff_codec_aiff_tags, par->codec_tag);
        if (par->codec_id == AV_CODEC_ID_NONE) {
            char tag[32];
            av_get_codec_tag_string(tag, sizeof(tag), par->codec_tag);
            avpriv_request_sample(s, "unknown or unsupported codec tag: %s", tag);
        }
        size -= 4;
    }

    if (version != AIFF_C_VERSION1 || par->codec_id == AV_CODEC_ID_PCM_S16BE) {
        par->codec_id = aiff_codec_get_id(par->bits_per_coded_sample);
        par->bits_per_coded_sample = av_get_bits_per_sample(par->codec_id);
        aiff->block_duration = 1;
    } else {
        switch (par->codec_id) {
        case AV_CODEC_ID_PCM_F32BE:
        case AV_CODEC_ID_PCM_F64BE:
        case AV_CODEC_ID_PCM_S16LE:
        case AV_CODEC_ID_PCM_ALAW:
        case AV_CODEC_ID_PCM_MULAW:
            aiff->block_duration = 1;
            break;
        case AV_CODEC_ID_ADPCM_IMA_QT:
            par->block_align = 34 * par->channels;
            break;
        case AV_CODEC_ID_MACE3:
            par->block_align = 2 * par->channels;
            break;
        case AV_CODEC_ID_ADPCM_G726LE:
            par->bits_per_coded_sample = 5;
        case AV_CODEC_ID_ADPCM_IMA_WS:
        case AV_CODEC_ID_ADPCM_G722:
        case AV_CODEC_ID_MACE6:
        case AV_CODEC_ID_SDX2_DPCM:
            par->block_align = 1 * par->channels;
            break;
        case AV_CODEC_ID_GSM:
            par->block_align = 33;
            break;
        default:
            aiff->block_duration = 1;
            break;
        }
        if (par->block_align > 0)
            aiff->block_duration = av_get_audio_frame_duration2(par,
                                                                par->block_align);
    }

    /
    if (!par->block_align)
        par->block_align = (av_get_bits_per_sample(par->codec_id) * par->channels) >> 3;

    if (aiff->block_duration) {
        par->bit_rate = par->sample_rate * (par->block_align << 3) /
                        aiff->block_duration;
    }

    /
    if (size)
        avio_skip(pb, size);

    return num_frames;
}
