static int read_sm_data(AVFormatContext *s, AVIOContext *bc, AVPacket *pkt, int is_meta, int64_t maxpos)
{
    int count = ffio_read_varlen(bc);
    int skip_start = 0;
    int skip_end = 0;
    int channels = 0;
    int64_t channel_layout = 0;
    int sample_rate = 0;
    int width = 0;
    int height = 0;
    int i, ret;

    for (i=0; i<count; i++) {
        uint8_t name[256], str_value[256], type_str[256];
        int value;
        if (avio_tell(bc) >= maxpos)
            return AVERROR_INVALIDDATA;
        ret = get_str(bc, name, sizeof(name));
        if (ret < 0) {
            av_log(s, AV_LOG_ERROR, "get_str failed while reading sm data\n");
            return ret;
        }
        value = get_s(bc);

        if (value == -1) {
            ret = get_str(bc, str_value, sizeof(str_value));
            if (ret < 0) {
                av_log(s, AV_LOG_ERROR, "get_str failed while reading sm data\n");
                return ret;
            }
            av_log(s, AV_LOG_WARNING, "Unknown string %s / %s\n", name, str_value);
        } else if (value == -2) {
            uint8_t *dst = NULL;
            int64_t v64, value_len;

            ret = get_str(bc, type_str, sizeof(type_str));
            if (ret < 0) {
                av_log(s, AV_LOG_ERROR, "get_str failed while reading sm data\n");
                return ret;
            }
            value_len = ffio_read_varlen(bc);
            if (avio_tell(bc) + value_len >= maxpos)
                return AVERROR_INVALIDDATA;
            if (!strcmp(name, "Palette")) {
                dst = av_packet_new_side_data(pkt, AV_PKT_DATA_PALETTE, value_len);
            } else if (!strcmp(name, "Extradata")) {
                dst = av_packet_new_side_data(pkt, AV_PKT_DATA_NEW_EXTRADATA, value_len);
            } else if (sscanf(name, "CodecSpecificSide%"SCNd64"", &v64) == 1) {
                dst = av_packet_new_side_data(pkt, AV_PKT_DATA_MATROSKA_BLOCKADDITIONAL, value_len + 8);
                if(!dst)
                    return AVERROR(ENOMEM);
                AV_WB64(dst, v64);
                dst += 8;
            } else if (!strcmp(name, "ChannelLayout") && value_len == 8) {
                channel_layout = avio_rl64(bc);
                continue;
            } else {
                av_log(s, AV_LOG_WARNING, "Unknown data %s / %s\n", name, type_str);
                avio_skip(bc, value_len);
                continue;
            }
            if(!dst)
                return AVERROR(ENOMEM);
            avio_read(bc, dst, value_len);
        } else if (value == -3) {
            value = get_s(bc);
        } else if (value == -4) {
            value = ffio_read_varlen(bc);
        } else if (value < -4) {
            get_s(bc);
        } else {
            if (!strcmp(name, "SkipStart")) {
                skip_start = value;
            } else if (!strcmp(name, "SkipEnd")) {
                skip_end = value;
            } else if (!strcmp(name, "Channels")) {
                channels = value;
            } else if (!strcmp(name, "SampleRate")) {
                sample_rate = value;
            } else if (!strcmp(name, "Width")) {
                width = value;
            } else if (!strcmp(name, "Height")) {
                height = value;
            } else {
                av_log(s, AV_LOG_WARNING, "Unknown integer %s\n", name);
            }
        }
    }

    if (channels || channel_layout || sample_rate || width || height) {
        uint8_t *dst = av_packet_new_side_data(pkt, AV_PKT_DATA_PARAM_CHANGE, 28);
        if (!dst)
            return AVERROR(ENOMEM);
        bytestream_put_le32(&dst,
                            AV_SIDE_DATA_PARAM_CHANGE_CHANNEL_COUNT*(!!channels) +
                            AV_SIDE_DATA_PARAM_CHANGE_CHANNEL_LAYOUT*(!!channel_layout) +
                            AV_SIDE_DATA_PARAM_CHANGE_SAMPLE_RATE*(!!sample_rate) +
                            AV_SIDE_DATA_PARAM_CHANGE_DIMENSIONS*(!!(width|height))
                           );
        if (channels)
            bytestream_put_le32(&dst, channels);
        if (channel_layout)
            bytestream_put_le64(&dst, channel_layout);
        if (sample_rate)
            bytestream_put_le32(&dst, sample_rate);
        if (width || height){
            bytestream_put_le32(&dst, width);
            bytestream_put_le32(&dst, height);
        }
    }

    if (skip_start || skip_end) {
        uint8_t *dst = av_packet_new_side_data(pkt, AV_PKT_DATA_SKIP_SAMPLES, 10);
        if (!dst)
            return AVERROR(ENOMEM);
        AV_WL32(dst, skip_start);
        AV_WL32(dst+4, skip_end);
    }

    if (avio_tell(bc) >= maxpos)
        return AVERROR_INVALIDDATA;

    return 0;
}
