static void rm_read_audio_stream_info(AVFormatContext *s, AVStream *st,
                                      int read_all)
{
    RMContext *rm = s->priv_data;
    ByteIOContext *pb = &s->pb;
    char buf[256];
    uint32_t version;
    int i;

    /
    version = get_be32(pb); /
    if (((version >> 16) & 0xff) == 3) {
        int64_t startpos = url_ftell(pb);
        /
        for(i = 0; i < 14; i++)
            get_byte(pb);
        get_str8(pb, s->title, sizeof(s->title));
        get_str8(pb, s->author, sizeof(s->author));
        get_str8(pb, s->copyright, sizeof(s->copyright));
        get_str8(pb, s->comment, sizeof(s->comment));
        if ((startpos + (version & 0xffff)) >= url_ftell(pb) + 2) {
        // fourcc (should always be "lpcJ")
        get_byte(pb);
        get_str8(pb, buf, sizeof(buf));
        }
        // Skip extra header crap (this should never happen)
        if ((startpos + (version & 0xffff)) > url_ftell(pb))
            url_fskip(pb, (version & 0xffff) + startpos - url_ftell(pb));
        st->codec->sample_rate = 8000;
        st->codec->channels = 1;
        st->codec->codec_type = CODEC_TYPE_AUDIO;
        st->codec->codec_id = CODEC_ID_RA_144;
    } else {
        int flavor, sub_packet_h, coded_framesize, sub_packet_size;
        /
        get_be32(pb); /
        get_be32(pb); /
        get_be16(pb); /
        get_be32(pb); /
        flavor= get_be16(pb); /
        rm->coded_framesize = coded_framesize = get_be32(pb); /
        get_be32(pb); /
        get_be32(pb); /
        get_be32(pb); /
        rm->sub_packet_h = sub_packet_h = get_be16(pb); /
        st->codec->block_align= get_be16(pb); /
        rm->sub_packet_size = sub_packet_size = get_be16(pb); /
        get_be16(pb); /
        if (((version >> 16) & 0xff) == 5) {
            get_be16(pb); get_be16(pb); get_be16(pb); }
        st->codec->sample_rate = get_be16(pb);
        get_be32(pb);
        st->codec->channels = get_be16(pb);
        if (((version >> 16) & 0xff) == 5) {
            get_be32(pb);
            buf[0] = get_byte(pb);
            buf[1] = get_byte(pb);
            buf[2] = get_byte(pb);
            buf[3] = get_byte(pb);
            buf[4] = 0;
        } else {
        get_str8(pb, buf, sizeof(buf)); /
        get_str8(pb, buf, sizeof(buf)); /
        }
        st->codec->codec_type = CODEC_TYPE_AUDIO;
        if (!strcmp(buf, "dnet")) {
            st->codec->codec_id = CODEC_ID_AC3;
        } else if (!strcmp(buf, "28_8")) {
            st->codec->codec_id = CODEC_ID_RA_288;
            st->codec->extradata_size= 0;
            rm->audio_framesize = st->codec->block_align;
            st->codec->block_align = coded_framesize;

            if(rm->audio_framesize >= UINT_MAX / sub_packet_h){
                av_log(s, AV_LOG_ERROR, "rm->audio_framesize * sub_packet_h too large\n");
                return -1;
            }

            rm->audiobuf = av_malloc(rm->audio_framesize * sub_packet_h);
        } else if (!strcmp(buf, "cook")) {
            int codecdata_length, i;
            get_be16(pb); get_byte(pb);
            if (((version >> 16) & 0xff) == 5)
                get_byte(pb);
            codecdata_length = get_be32(pb);
            if(codecdata_length + FF_INPUT_BUFFER_PADDING_SIZE <= (unsigned)codecdata_length){
                av_log(s, AV_LOG_ERROR, "codecdata_length too large\n");
                return -1;
            }

            st->codec->codec_id = CODEC_ID_COOK;
            st->codec->extradata_size= codecdata_length;
            st->codec->extradata= av_mallocz(st->codec->extradata_size + FF_INPUT_BUFFER_PADDING_SIZE);
            for(i = 0; i < codecdata_length; i++)
                ((uint8_t*)st->codec->extradata)[i] = get_byte(pb);
            rm->audio_framesize = st->codec->block_align;
            st->codec->block_align = rm->sub_packet_size;

            if(rm->audio_framesize >= UINT_MAX / sub_packet_h){
                av_log(s, AV_LOG_ERROR, "rm->audio_framesize * sub_packet_h too large\n");
                return -1;
            }

            rm->audiobuf = av_malloc(rm->audio_framesize * sub_packet_h);
        } else if (!strcmp(buf, "raac") || !strcmp(buf, "racp")) {
            int codecdata_length, i;
            get_be16(pb); get_byte(pb);
            if (((version >> 16) & 0xff) == 5)
                get_byte(pb);
            st->codec->codec_id = CODEC_ID_AAC;
            codecdata_length = get_be32(pb);
            if (codecdata_length >= 1) {
                st->codec->extradata_size = codecdata_length - 1;
                st->codec->extradata = av_mallocz(st->codec->extradata_size + FF_INPUT_BUFFER_PADDING_SIZE);
                get_byte(pb);
                for(i = 0; i < st->codec->extradata_size; i++)
                    ((uint8_t*)st->codec->extradata)[i] = get_byte(pb);
            }
        } else {
            st->codec->codec_id = CODEC_ID_NONE;
            pstrcpy(st->codec->codec_name, sizeof(st->codec->codec_name),
                    buf);
        }
        if (read_all) {
            get_byte(pb);
            get_byte(pb);
            get_byte(pb);

            get_str8(pb, s->title, sizeof(s->title));
            get_str8(pb, s->author, sizeof(s->author));
            get_str8(pb, s->copyright, sizeof(s->copyright));
            get_str8(pb, s->comment, sizeof(s->comment));
        }
    }
}
