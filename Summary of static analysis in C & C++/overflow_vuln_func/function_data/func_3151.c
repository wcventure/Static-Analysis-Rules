static int sox_read_header(AVFormatContext *s)
{
    AVIOContext *pb = s->pb;
    unsigned header_size, comment_size;
    double sample_rate, sample_rate_frac;
    AVStream *st;

    st = avformat_new_stream(s, NULL);
    if (!st)
        return AVERROR(ENOMEM);

    st->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;

    if (avio_rl32(pb) == SOX_TAG) {
        st->codecpar->codec_id = AV_CODEC_ID_PCM_S32LE;
        header_size         = avio_rl32(pb);
        avio_skip(pb, 8); /
        sample_rate         = av_int2double(avio_rl64(pb));
        st->codecpar->channels = avio_rl32(pb);
        comment_size        = avio_rl32(pb);
    } else {
        st->codecpar->codec_id = AV_CODEC_ID_PCM_S32BE;
        header_size         = avio_rb32(pb);
        avio_skip(pb, 8); /
        sample_rate         = av_int2double(avio_rb64(pb));
        st->codecpar->channels = avio_rb32(pb);
        comment_size        = avio_rb32(pb);
    }

    if (comment_size > 0xFFFFFFFFU - SOX_FIXED_HDR - 4U) {
        av_log(s, AV_LOG_ERROR, "invalid comment size (%u)\n", comment_size);
        return AVERROR_INVALIDDATA;
    }

    if (sample_rate <= 0 || sample_rate > INT_MAX) {
        av_log(s, AV_LOG_ERROR, "invalid sample rate (%f)\n", sample_rate);
        return AVERROR_INVALIDDATA;
    }

    sample_rate_frac = sample_rate - floor(sample_rate);
    if (sample_rate_frac)
        av_log(s, AV_LOG_WARNING,
               "truncating fractional part of sample rate (%f)\n",
               sample_rate_frac);

    if ((header_size + 4) & 7 || header_size < SOX_FIXED_HDR + comment_size
        || st->codecpar->channels > 65535) / {
        av_log(s, AV_LOG_ERROR, "invalid header\n");
        return AVERROR_INVALIDDATA;
    }

    if (comment_size && comment_size < UINT_MAX) {
        char *comment = av_malloc(comment_size+1);
        if(!comment)
            return AVERROR(ENOMEM);
        if (avio_read(pb, comment, comment_size) != comment_size) {
            av_freep(&comment);
            return AVERROR(EIO);
        }
        comment[comment_size] = 0;

        av_dict_set(&s->metadata, "comment", comment,
                               AV_DICT_DONT_STRDUP_VAL);
    }

    avio_skip(pb, header_size - SOX_FIXED_HDR - comment_size);

    st->codecpar->sample_rate           = sample_rate;
    st->codecpar->bits_per_coded_sample = 32;
    st->codecpar->bit_rate              = st->codecpar->sample_rate *
                                          st->codecpar->bits_per_coded_sample *
                                          st->codecpar->channels;
    st->codecpar->block_align           = st->codecpar->bits_per_coded_sample *
                                          st->codecpar->channels / 8;

    avpriv_set_pts_info(st, 64, 1, st->codecpar->sample_rate);

    return 0;
}
