static int asf_read_header(AVFormatContext *s, AVFormatParameters *ap)
{
    ASFContext *asf = s->priv_data;
    GUID g;
    ByteIOContext *pb = &s->pb;
    AVStream *st;
    ASFStream *asf_st;
    int size, i;
    int64_t gsize;

    get_guid(pb, &g);
    if (memcmp(&g, &asf_header, sizeof(GUID)))
        goto fail;
    get_le64(pb);
    get_le32(pb);
    get_byte(pb);
    get_byte(pb);
    memset(&asf->asfid2avid, -1, sizeof(asf->asfid2avid));
    for(;;) {
        get_guid(pb, &g);
        gsize = get_le64(pb);
#ifdef DEBUG
        printf("%08"PRIx64": ", url_ftell(pb) - 24);
        print_guid(&g);
        printf("  size=0x%"PRIx64"\n", gsize);
#endif
        if (gsize < 24)
            goto fail;
        if (!memcmp(&g, &file_header, sizeof(GUID))) {
            get_guid(pb, &asf->hdr.guid);
            asf->hdr.file_size          = get_le64(pb);
            asf->hdr.create_time        = get_le64(pb);
            asf->hdr.packets_count      = get_le64(pb);
            asf->hdr.send_time          = get_le64(pb);
            asf->hdr.play_time          = get_le64(pb);
            asf->hdr.preroll            = get_le32(pb);
            asf->hdr.ignore             = get_le32(pb);
            asf->hdr.flags              = get_le32(pb);
            asf->hdr.min_pktsize        = get_le32(pb);
            asf->hdr.max_pktsize        = get_le32(pb);
            asf->hdr.max_bitrate        = get_le32(pb);
            asf->packet_size = asf->hdr.max_pktsize;
            asf->nb_packets = asf->hdr.packets_count;
        } else if (!memcmp(&g, &stream_header, sizeof(GUID))) {
            int type, type_specific_size, sizeX;
            uint64_t total_size;
            unsigned int tag1;
            int64_t pos1, pos2;
            int test_for_ext_stream_audio;

            pos1 = url_ftell(pb);

            st = av_new_stream(s, 0);
            if (!st)
                goto fail;
            av_set_pts_info(st, 32, 1, 1000); /
            asf_st = av_mallocz(sizeof(ASFStream));
            if (!asf_st)
                goto fail;
            st->priv_data = asf_st;
            st->start_time = asf->hdr.preroll;
            if(!(asf->hdr.flags & 0x01)) { // if we aren't streaming...
                st->duration = asf->hdr.send_time /
                    (10000000 / 1000) - st->start_time;
            }
            get_guid(pb, &g);

            test_for_ext_stream_audio = 0;
            if (!memcmp(&g, &audio_stream, sizeof(GUID))) {
                type = CODEC_TYPE_AUDIO;
            } else if (!memcmp(&g, &video_stream, sizeof(GUID))) {
                type = CODEC_TYPE_VIDEO;
            } else if (!memcmp(&g, &command_stream, sizeof(GUID))) {
                type = CODEC_TYPE_UNKNOWN;
            } else if (!memcmp(&g, &ext_stream_embed_stream_header, sizeof(GUID))) {
                test_for_ext_stream_audio = 1;
                type = CODEC_TYPE_UNKNOWN;
            } else {
                goto fail;
            }
            get_guid(pb, &g);
            total_size = get_le64(pb);
            type_specific_size = get_le32(pb);
            get_le32(pb);
            st->id = get_le16(pb) & 0x7f; /
            // mapping of asf ID to AV stream ID;
            asf->asfid2avid[st->id] = s->nb_streams - 1;

            get_le32(pb);

            if (test_for_ext_stream_audio) {
                get_guid(pb, &g);
                if (!memcmp(&g, &ext_stream_audio_stream, sizeof(GUID))) {
                    type = CODEC_TYPE_AUDIO;
                    get_guid(pb, &g);
                    get_le32(pb);
                    get_le32(pb);
                    get_le32(pb);
                    get_guid(pb, &g);
                    get_le32(pb);
                }
            }

            st->codec->codec_type = type;
            if (type == CODEC_TYPE_AUDIO) {
                get_wav_header(pb, st->codec, type_specific_size);
                st->need_parsing = 1;
                /
                pos2 = url_ftell(pb);
                if (gsize > (pos2 + 8 - pos1 + 24)) {
                    asf_st->ds_span = get_byte(pb);
                    asf_st->ds_packet_size = get_le16(pb);
                    asf_st->ds_chunk_size = get_le16(pb);
                    asf_st->ds_data_size = get_le16(pb);
                    asf_st->ds_silence_data = get_byte(pb);
                }
                //printf("Descrambling: ps:%d cs:%d ds:%d s:%d  sd:%d\n",
                //       asf_st->ds_packet_size, asf_st->ds_chunk_size,
                //       asf_st->ds_data_size, asf_st->ds_span, asf_st->ds_silence_data);
                if (asf_st->ds_span > 1) {
                    if (!asf_st->ds_chunk_size
                        || (asf_st->ds_packet_size/asf_st->ds_chunk_size <= 1))
                        asf_st->ds_span = 0; // disable descrambling
                }
                switch (st->codec->codec_id) {
                case CODEC_ID_MP3:
                    st->codec->frame_size = MPA_FRAME_SIZE;
                    break;
                case CODEC_ID_PCM_S16LE:
                case CODEC_ID_PCM_S16BE:
                case CODEC_ID_PCM_U16LE:
                case CODEC_ID_PCM_U16BE:
                case CODEC_ID_PCM_S8:
                case CODEC_ID_PCM_U8:
                case CODEC_ID_PCM_ALAW:
                case CODEC_ID_PCM_MULAW:
                    st->codec->frame_size = 1;
                    break;
                default:
                    /
                    st->codec->frame_size = 1;
                    break;
                }
            } else if (type == CODEC_TYPE_VIDEO) {
                get_le32(pb);
                get_le32(pb);
                get_byte(pb);
                size = get_le16(pb); /
                sizeX= get_le32(pb); /
                st->codec->width = get_le32(pb);
                st->codec->height = get_le32(pb);
                /
                get_le16(pb); /
                st->codec->bits_per_sample = get_le16(pb); /
                tag1 = get_le32(pb);
                url_fskip(pb, 20);
//                av_log(NULL, AV_LOG_DEBUG, "size:%d tsize:%d sizeX:%d\n", size, total_size, sizeX);
                size= sizeX;
                if (size > 40) {
                    st->codec->extradata_size = size - 40;
                    st->codec->extradata = av_mallocz(st->codec->extradata_size + FF_INPUT_BUFFER_PADDING_SIZE);
                    get_buffer(pb, st->codec->extradata, st->codec->extradata_size);
                }

        /
        /
        /
        if (st->codec->extradata_size && (st->codec->bits_per_sample <= 8)) {
            st->codec->palctrl = av_mallocz(sizeof(AVPaletteControl));
#ifdef WORDS_BIGENDIAN
            for (i = 0; i < FFMIN(st->codec->extradata_size, AVPALETTE_SIZE)/4; i++)
                st->codec->palctrl->palette[i] = bswap_32(((uint32_t*)st->codec->extradata)[i]);
#else
            memcpy(st->codec->palctrl->palette, st->codec->extradata,
                   FFMIN(st->codec->extradata_size, AVPALETTE_SIZE));
#endif
            st->codec->palctrl->palette_changed = 1;
        }

                st->codec->codec_tag = tag1;
                st->codec->codec_id = codec_get_id(codec_bmp_tags, tag1);
                if(tag1 == MKTAG('D', 'V', 'R', ' '))
                    st->need_parsing = 1;
            }
            pos2 = url_ftell(pb);
            url_fskip(pb, gsize - (pos2 - pos1 + 24));
        } else if (!memcmp(&g, &data_header, sizeof(GUID))) {
            asf->data_object_offset = url_ftell(pb);
            // if not streaming, gsize is not unlimited (how?), and there is enough space in the file..
            if (!(asf->hdr.flags & 0x01) && gsize != (uint64_t)-1 && gsize >= 24) {
                asf->data_object_size = gsize - 24;
            } else {
                asf->data_object_size = (uint64_t)-1;
            }
            break;
        } else if (!memcmp(&g, &comment_header, sizeof(GUID))) {
            int len1, len2, len3, len4, len5;

            len1 = get_le16(pb);
            len2 = get_le16(pb);
            len3 = get_le16(pb);
            len4 = get_le16(pb);
            len5 = get_le16(pb);
            get_str16_nolen(pb, len1, s->title, sizeof(s->title));
            get_str16_nolen(pb, len2, s->author, sizeof(s->author));
            get_str16_nolen(pb, len3, s->copyright, sizeof(s->copyright));
            get_str16_nolen(pb, len4, s->comment, sizeof(s->comment));
            url_fskip(pb, len5);
       } else if (!memcmp(&g, &extended_content_header, sizeof(GUID))) {
                int desc_count, i;

                desc_count = get_le16(pb);
                for(i=0;i<desc_count;i++)
                {
                        int name_len,value_type,value_len;
                        uint64_t value_num = 0;
                        char *name, *value;

                        name_len = get_le16(pb);
                        name = av_malloc(name_len * 2);
                        get_str16_nolen(pb, name_len, name, name_len * 2);
                        value_type = get_le16(pb);
                        value_len = get_le16(pb);
                        if ((value_type == 0) || (value_type == 1)) // unicode or byte
                        {
                                value = av_malloc(value_len * 2);
                                get_str16_nolen(pb, value_len, value,
                                        value_len * 2);
                                if (strcmp(name,"WM/AlbumTitle")==0) { pstrcpy(s->album, sizeof(s->album), value); }
                                if (strcmp(name,"WM/Genre")==0) { pstrcpy(s->genre, sizeof(s->genre), value); }
                                av_free(value);
                        }
                        if ((value_type >= 2) && (value_type <= 5)) // boolean or DWORD or QWORD or WORD
                        {
                                if (value_type==2) value_num = get_le32(pb);
                                if (value_type==3) value_num = get_le32(pb);
                                if (value_type==4) value_num = get_le64(pb);
                                if (value_type==5) value_num = get_le16(pb);
                                if (strcmp(name,"WM/Track")==0) s->track = value_num + 1;
                                if (strcmp(name,"WM/TrackNumber")==0) s->track = value_num;
                        }
                        av_free(name);
                }
        } else if (!memcmp(&g, &ext_stream_header, sizeof(GUID))) {
            int ext_len, payload_ext_ct, stream_ct;
            uint32_t ext_d;
            int64_t pos_ex_st;
            pos_ex_st = url_ftell(pb);

            get_le64(pb);
            get_le64(pb);
            get_le32(pb);
            get_le32(pb);
            get_le32(pb);
            get_le32(pb);
            get_le32(pb);
            get_le32(pb);
            get_le32(pb);
            get_le32(pb);
            get_le16(pb);
            get_le16(pb);
            get_le64(pb);
            stream_ct = get_le16(pb);
            payload_ext_ct = get_le16(pb);

            for (i=0; i<stream_ct; i++){
                get_le16(pb);
                ext_len = get_le16(pb);
                url_fseek(pb, ext_len, SEEK_CUR);
            }

            for (i=0; i<payload_ext_ct; i++){
                get_guid(pb, &g);
                ext_d=get_le16(pb);
                ext_len=get_le32(pb);
                url_fseek(pb, ext_len, SEEK_CUR);
            }

            // there could be a optional stream properties object to follow
            // if so the next iteration will pick it up
        } else if (!memcmp(&g, &head1_guid, sizeof(GUID))) {
            int v1, v2;
            get_guid(pb, &g);
            v1 = get_le32(pb);
            v2 = get_le16(pb);
#if 0
        } else if (!memcmp(&g, &codec_comment_header, sizeof(GUID))) {
            int len, v1, n, num;
            char str[256], *q;
            char tag[16];

            get_guid(pb, &g);
            print_guid(&g);

            n = get_le32(pb);
            for(i=0;i<n;i++) {
                num = get_le16(pb); /
                get_str16(pb, str, sizeof(str));
                get_str16(pb, str, sizeof(str));
                len = get_le16(pb);
                q = tag;
                while (len > 0) {
                    v1 = get_byte(pb);
                    if ((q - tag) < sizeof(tag) - 1)
                        *q++ = v1;
                    len--;
                }
                *q = '\0';
            }
#endif
        } else if (url_feof(pb)) {
            goto fail;
        } else {
            url_fseek(pb, gsize - 24, SEEK_CUR);
        }
    }
    get_guid(pb, &g);
    get_le64(pb);
    get_byte(pb);
    get_byte(pb);
    if (url_feof(pb))
        goto fail;
    asf->data_offset = url_ftell(pb);
    asf->packet_size_left = 0;

    return 0;

 fail:
     for(i=0;i<s->nb_streams;i++) {
        AVStream *st = s->streams[i];
        if (st) {
            av_free(st->priv_data);
            av_free(st->codec->extradata);
        }
        av_free(st);
    }
    return -1;
}
