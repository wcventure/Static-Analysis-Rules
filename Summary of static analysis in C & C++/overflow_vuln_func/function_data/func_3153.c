static int vqf_read_header(AVFormatContext *s)
{
    VqfContext *c = s->priv_data;
    AVStream *st  = avformat_new_stream(s, NULL);
    int chunk_tag;
    int rate_flag = -1;
    int header_size;
    int read_bitrate = 0;
    int size;
    uint8_t comm_chunk[12];

    if (!st)
        return AVERROR(ENOMEM);

    avio_skip(s->pb, 12);

    header_size = avio_rb32(s->pb);

    st->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
    st->codecpar->codec_id   = AV_CODEC_ID_TWINVQ;
    st->start_time = 0;

    do {
        int len;
        chunk_tag = avio_rl32(s->pb);

        if (chunk_tag == MKTAG('D','A','T','A'))
            break;

        len = avio_rb32(s->pb);

        if ((unsigned) len > INT_MAX/2) {
            av_log(s, AV_LOG_ERROR, "Malformed header\n");
            return -1;
        }

        header_size -= 8;

        switch(chunk_tag){
        case MKTAG('C','O','M','M'):
            avio_read(s->pb, comm_chunk, 12);
            st->codecpar->channels = AV_RB32(comm_chunk    ) + 1;
            read_bitrate        = AV_RB32(comm_chunk + 4);
            rate_flag           = AV_RB32(comm_chunk + 8);
            avio_skip(s->pb, len-12);

            if (st->codecpar->channels <= 0) {
                av_log(s, AV_LOG_ERROR, "Invalid number of channels\n");
                return AVERROR_INVALIDDATA;
            }

            st->codecpar->bit_rate = read_bitrate * 1000;
            break;
        case MKTAG('D','S','I','Z'): // size of compressed data
        {
            av_dict_set_int(&s->metadata, "size", avio_rb32(s->pb), 0);
        }
            break;
        case MKTAG('Y','E','A','R'): // recording date
        case MKTAG('E','N','C','D'): // compression date
        case MKTAG('E','X','T','R'): // reserved
        case MKTAG('_','Y','M','H'): // reserved
        case MKTAG('_','N','T','T'): // reserved
        case MKTAG('_','I','D','3'): // reserved for ID3 tags
            avio_skip(s->pb, FFMIN(len, header_size));
            break;
        default:
            add_metadata(s, chunk_tag, len, header_size);
            break;
        }

        header_size -= len;

    } while (header_size >= 0 && !avio_feof(s->pb));

    switch (rate_flag) {
    case -1:
        av_log(s, AV_LOG_ERROR, "COMM tag not found!\n");
        return -1;
    case 44:
        st->codecpar->sample_rate = 44100;
        break;
    case 22:
        st->codecpar->sample_rate = 22050;
        break;
    case 11:
        st->codecpar->sample_rate = 11025;
        break;
    default:
        if (rate_flag < 8 || rate_flag > 44) {
            av_log(s, AV_LOG_ERROR, "Invalid rate flag %d\n", rate_flag);
            return AVERROR_INVALIDDATA;
        }
        st->codecpar->sample_rate = rate_flag*1000;
        break;
    }

    if (read_bitrate / st->codecpar->channels <  8 ||
        read_bitrate / st->codecpar->channels > 48) {
        av_log(s, AV_LOG_ERROR, "Invalid bitrate per channel %d\n",
               read_bitrate / st->codecpar->channels);
        return AVERROR_INVALIDDATA;
    }

    switch (((st->codecpar->sample_rate/1000) << 8) +
            read_bitrate/st->codecpar->channels) {
    case (11<<8) + 8 :
    case (8 <<8) + 8 :
    case (11<<8) + 10:
    case (22<<8) + 32:
        size = 512;
        break;
    case (16<<8) + 16:
    case (22<<8) + 20:
    case (22<<8) + 24:
        size = 1024;
        break;
    case (44<<8) + 40:
    case (44<<8) + 48:
        size = 2048;
        break;
    default:
        av_log(s, AV_LOG_ERROR, "Mode not supported: %d Hz, %"PRId64" kb/s.\n",
               st->codecpar->sample_rate, (int64_t)st->codecpar->bit_rate);
        return -1;
    }
    c->frame_bit_len = st->codecpar->bit_rate*size/st->codecpar->sample_rate;
    avpriv_set_pts_info(st, 64, size, st->codecpar->sample_rate);

    /
    if (ff_alloc_extradata(st->codecpar, 12))
        return AVERROR(ENOMEM);
    memcpy(st->codecpar->extradata, comm_chunk, 12);

    ff_metadata_conv_ctx(s, NULL, vqf_metadata_conv);

    return 0;
}
