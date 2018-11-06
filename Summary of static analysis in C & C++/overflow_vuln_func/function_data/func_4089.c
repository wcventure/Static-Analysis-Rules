int ff_get_wav_header(AVFormatContext *s, AVIOContext *pb,
                      AVCodecContext *codec, int size, int big_endian)
{
    int id;
    uint64_t bitrate;

    if (size < 14) {
        avpriv_request_sample(codec, "wav header size < 14");
        return AVERROR_INVALIDDATA;
    }

    codec->codec_type  = AVMEDIA_TYPE_AUDIO;
    if (!big_endian) {
        id                 = avio_rl16(pb);
        codec->channels    = avio_rl16(pb);
        codec->sample_rate = avio_rl32(pb);
        bitrate            = avio_rl32(pb) * 8;
        codec->block_align = avio_rl16(pb);
    } else {
        id                 = avio_rb16(pb);
        codec->channels    = avio_rb16(pb);
        codec->sample_rate = avio_rb32(pb);
        bitrate            = avio_rb32(pb) * 8;
        codec->block_align = avio_rb16(pb);
    }
    if (size == 14) {  /
        codec->bits_per_coded_sample = 8;
    } else {
        if (!big_endian) {
            codec->bits_per_coded_sample = avio_rl16(pb);
        } else {
            codec->bits_per_coded_sample = avio_rb16(pb);
        }
    }
    if (id == 0xFFFE) {
        codec->codec_tag = 0;
    } else {
        codec->codec_tag = id;
        codec->codec_id  = ff_wav_codec_get_id(id,
                                               codec->bits_per_coded_sample);
    }
    if (size >= 18) {  /
        int cbSize = avio_rl16(pb); /
        if (big_endian) {
            avpriv_report_missing_feature(codec, "WAVEFORMATEX support for RIFX files\n");
            return AVERROR_PATCHWELCOME;
        }
        size  -= 18;
        cbSize = FFMIN(size, cbSize);
        if (cbSize >= 22 && id == 0xfffe) { /
            parse_waveformatex(pb, codec);
            cbSize -= 22;
            size   -= 22;
        }
        if (cbSize > 0) {
            av_freep(&codec->extradata);
            if (ff_get_extradata(codec, pb, cbSize) < 0)
                return AVERROR(ENOMEM);
            size -= cbSize;
        }

        /
        if (size > 0)
            avio_skip(pb, size);
    }

    if (bitrate > INT_MAX) {
        if (s->error_recognition & AV_EF_EXPLODE) {
            av_log(s, AV_LOG_ERROR,
                   "The bitrate %"PRIu64" is too large.\n",
                    bitrate);
            return AVERROR_INVALIDDATA;
        } else {
            av_log(s, AV_LOG_WARNING,
                   "The bitrate %"PRIu64" is too large, resetting to 0.",
                   bitrate);
            codec->bit_rate = 0;
        }
    } else {
        codec->bit_rate = bitrate;
    }

    if (codec->sample_rate <= 0) {
        av_log(s, AV_LOG_ERROR,
               "Invalid sample rate: %d\n", codec->sample_rate);
        return AVERROR_INVALIDDATA;
    }
    if (codec->codec_id == AV_CODEC_ID_AAC_LATM) {
        /
        codec->channels    = 0;
        codec->sample_rate = 0;
    }
    /
    if (codec->codec_id == AV_CODEC_ID_ADPCM_G726 && codec->sample_rate)
        codec->bits_per_coded_sample = codec->bit_rate / codec->sample_rate;

    return 0;
}
