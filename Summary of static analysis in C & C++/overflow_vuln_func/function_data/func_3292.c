int ff_rm_read_mdpr_codecdata(AVFormatContext *s, AVIOContext *pb,
                              AVStream *st, RMStream *rst,
                              unsigned int codec_data_size, const uint8_t *mime)
{
    unsigned int v;
    int size;
    int64_t codec_pos;
    int ret;

    if (codec_data_size > INT_MAX)
        return AVERROR_INVALIDDATA;

    avpriv_set_pts_info(st, 64, 1, 1000);
    codec_pos = avio_tell(pb);
    v = avio_rb32(pb);

    if (v == MKBETAG('M', 'L', 'T', 'I')) {
        int number_of_streams = avio_rb16(pb);
        int number_of_mdpr;
        int i;
        for (i = 0; i<number_of_streams; i++)
            avio_rb16(pb);
        number_of_mdpr = avio_rb16(pb);
        if (number_of_mdpr != 1) {
            avpriv_request_sample(s, "MLTI with multiple MDPR");
        }
        avio_rb32(pb);
        v = avio_rb32(pb);
    }

    if (v == MKTAG(0xfd, 'a', 'r', '.')) {
        /
        if (rm_read_audio_stream_info(s, pb, st, rst, 0))
            return -1;
    } else if (v == MKBETAG('L', 'S', 'D', ':')) {
        avio_seek(pb, -4, SEEK_CUR);
        if ((ret = rm_read_extradata(pb, st->codec, codec_data_size)) < 0)
            return ret;

        st->codec->codec_type = AVMEDIA_TYPE_AUDIO;
        st->codec->codec_tag  = AV_RL32(st->codec->extradata);
        st->codec->codec_id   = ff_codec_get_id(ff_rm_codec_tags,
                                                st->codec->codec_tag);
    } else if(mime && !strcmp(mime, "logical-fileinfo")){
        int stream_count, rule_count, property_count, i;
        ff_free_stream(s, st);
        if (avio_rb16(pb) != 0) {
            av_log(s, AV_LOG_WARNING, "Unsupported version\n");
            goto skip;
        }
        stream_count = avio_rb16(pb);
        avio_skip(pb, 6*stream_count);
        rule_count = avio_rb16(pb);
        avio_skip(pb, 2*rule_count);
        property_count = avio_rb16(pb);
        for(i=0; i<property_count; i++){
            uint8_t name[128], val[128];
            avio_rb32(pb);
            if (avio_rb16(pb) != 0) {
                av_log(s, AV_LOG_WARNING, "Unsupported Name value property version\n");
                goto skip; //FIXME skip just this one
            }
            get_str8(pb, name, sizeof(name));
            switch(avio_rb32(pb)) {
            case 2: get_strl(pb, val, sizeof(val), avio_rb16(pb));
                av_dict_set(&s->metadata, name, val, 0);
                break;
            default: avio_skip(pb, avio_rb16(pb));
            }
        }
    } else {
        int fps;
        if (avio_rl32(pb) != MKTAG('V', 'I', 'D', 'O')) {
        fail1:
            av_log(s, AV_LOG_WARNING, "Unsupported stream type %08x\n", v);
            goto skip;
        }
        st->codec->codec_tag = avio_rl32(pb);
        st->codec->codec_id  = ff_codec_get_id(ff_rm_codec_tags,
                                               st->codec->codec_tag);
        av_dlog(s, "%X %X\n", st->codec->codec_tag, MKTAG('R', 'V', '2', '0'));
        if (st->codec->codec_id == AV_CODEC_ID_NONE)
            goto fail1;
        st->codec->width  = avio_rb16(pb);
        st->codec->height = avio_rb16(pb);
        avio_skip(pb, 2); // looks like bits per sample
        avio_skip(pb, 4); // always zero?
        st->codec->codec_type = AVMEDIA_TYPE_VIDEO;
        st->need_parsing = AVSTREAM_PARSE_TIMESTAMPS;
        fps = avio_rb32(pb);

        if ((ret = rm_read_extradata(pb, st->codec, codec_data_size - (avio_tell(pb) - codec_pos))) < 0)
            return ret;

        if (fps > 0) {
            av_reduce(&st->avg_frame_rate.den, &st->avg_frame_rate.num,
                      0x10000, fps, (1 << 30) - 1);
#if FF_API_R_FRAME_RATE
            st->r_frame_rate = st->avg_frame_rate;
#endif
        } else if (s->error_recognition & AV_EF_EXPLODE) {
            av_log(s, AV_LOG_ERROR, "Invalid framerate\n");
            return AVERROR_INVALIDDATA;
        }
    }

skip:
    /
    size = avio_tell(pb) - codec_pos;
    avio_skip(pb, codec_data_size - size);

    return 0;
}
