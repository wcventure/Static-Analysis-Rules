static int matroska_read_header(AVFormatContext *s)
{
    MatroskaDemuxContext *matroska = s->priv_data;
    EbmlList *attachements_list = &matroska->attachments;
    MatroskaAttachement *attachements;
    EbmlList *chapters_list = &matroska->chapters;
    MatroskaChapter *chapters;
    MatroskaTrack *tracks;
    uint64_t max_start = 0;
    int64_t pos;
    Ebml ebml = { 0 };
    AVStream *st;
    int i, j, k, res;

    matroska->ctx = s;

    /
    if (ebml_parse(matroska, ebml_syntax, &ebml)
        || ebml.version > EBML_VERSION       || ebml.max_size > sizeof(uint64_t)
        || ebml.id_length > sizeof(uint32_t) || ebml.doctype_version > 3 || !ebml.doctype) {
        av_log(matroska->ctx, AV_LOG_ERROR,
               "EBML header using unsupported features\n"
               "(EBML version %"PRIu64", doctype %s, doc version %"PRIu64")\n",
               ebml.version, ebml.doctype, ebml.doctype_version);
        ebml_free(ebml_syntax, &ebml);
        return AVERROR_PATCHWELCOME;
    } else if (ebml.doctype_version == 3) {
        av_log(matroska->ctx, AV_LOG_WARNING,
               "EBML header using unsupported features\n"
               "(EBML version %"PRIu64", doctype %s, doc version %"PRIu64")\n",
               ebml.version, ebml.doctype, ebml.doctype_version);
    }
    for (i = 0; i < FF_ARRAY_ELEMS(matroska_doctypes); i++)
        if (!strcmp(ebml.doctype, matroska_doctypes[i]))
            break;
    if (i >= FF_ARRAY_ELEMS(matroska_doctypes)) {
        av_log(s, AV_LOG_WARNING, "Unknown EBML doctype '%s'\n", ebml.doctype);
        if (matroska->ctx->error_recognition & AV_EF_EXPLODE) {
            ebml_free(ebml_syntax, &ebml);
            return AVERROR_INVALIDDATA;
        }
    }
    ebml_free(ebml_syntax, &ebml);

    /
    pos = avio_tell(matroska->ctx->pb);
    res = ebml_parse(matroska, matroska_segments, matroska);
    // try resyncing until we find a EBML_STOP type element.
    while (res != 1) {
        res = matroska_resync(matroska, pos);
        if (res < 0)
            return res;
        pos = avio_tell(matroska->ctx->pb);
        res = ebml_parse(matroska, matroska_segment, matroska);
    }
    matroska_execute_seekhead(matroska);

    if (!matroska->time_scale)
        matroska->time_scale = 1000000;
    if (matroska->duration)
        matroska->ctx->duration = matroska->duration * matroska->time_scale
                                  * 1000 / AV_TIME_BASE;
    av_dict_set(&s->metadata, "title", matroska->title, 0);

    if (matroska->date_utc.size == 8)
        matroska_metadata_creation_time(&s->metadata, AV_RB64(matroska->date_utc.data));

    tracks = matroska->tracks.elem;
    for (i=0; i < matroska->tracks.nb_elem; i++) {
        MatroskaTrack *track = &tracks[i];
        enum AVCodecID codec_id = AV_CODEC_ID_NONE;
        EbmlList *encodings_list = &track->encodings;
        MatroskaTrackEncoding *encodings = encodings_list->elem;
        uint8_t *extradata = NULL;
        int extradata_size = 0;
        int extradata_offset = 0;
        uint32_t fourcc = 0;
        AVIOContext b;

        /
        if (track->type != MATROSKA_TRACK_TYPE_VIDEO &&
            track->type != MATROSKA_TRACK_TYPE_AUDIO &&
            track->type != MATROSKA_TRACK_TYPE_SUBTITLE) {
            av_log(matroska->ctx, AV_LOG_INFO,
                   "Unknown or unsupported track type %"PRIu64"\n",
                   track->type);
            continue;
        }
        if (track->codec_id == NULL)
            continue;

        if (track->type == MATROSKA_TRACK_TYPE_VIDEO) {
            if (!track->default_duration && track->video.frame_rate > 0)
                track->default_duration = 1000000000/track->video.frame_rate;
            if (!track->video.display_width)
                track->video.display_width = track->video.pixel_width;
            if (!track->video.display_height)
                track->video.display_height = track->video.pixel_height;
            if (track->video.color_space.size == 4)
                fourcc = AV_RL32(track->video.color_space.data);
        } else if (track->type == MATROSKA_TRACK_TYPE_AUDIO) {
            if (!track->audio.out_samplerate)
                track->audio.out_samplerate = track->audio.samplerate;
        }
        if (encodings_list->nb_elem > 1) {
            av_log(matroska->ctx, AV_LOG_ERROR,
                   "Multiple combined encodings not supported");
        } else if (encodings_list->nb_elem == 1) {
            if (encodings[0].type ||
                (
#if CONFIG_ZLIB
                 encodings[0].compression.algo != MATROSKA_TRACK_ENCODING_COMP_ZLIB &&
#endif
#if CONFIG_BZLIB
                 encodings[0].compression.algo != MATROSKA_TRACK_ENCODING_COMP_BZLIB &&
#endif
#if CONFIG_LZO
                 encodings[0].compression.algo != MATROSKA_TRACK_ENCODING_COMP_LZO &&
#endif
                 encodings[0].compression.algo != MATROSKA_TRACK_ENCODING_COMP_HEADERSTRIP)) {
                encodings[0].scope = 0;
                av_log(matroska->ctx, AV_LOG_ERROR,
                       "Unsupported encoding type");
            } else if (track->codec_priv.size && encodings[0].scope&2) {
                uint8_t *codec_priv = track->codec_priv.data;
                int ret = matroska_decode_buffer(&track->codec_priv.data,
                                                 &track->codec_priv.size,
                                                 track);
                if (ret < 0) {
                    track->codec_priv.data = NULL;
                    track->codec_priv.size = 0;
                    av_log(matroska->ctx, AV_LOG_ERROR,
                           "Failed to decode codec private data\n");
                }

                if (codec_priv != track->codec_priv.data)
                    av_free(codec_priv);
            }
        }

        for(j=0; ff_mkv_codec_tags[j].id != AV_CODEC_ID_NONE; j++){
            if(!strncmp(ff_mkv_codec_tags[j].str, track->codec_id,
                        strlen(ff_mkv_codec_tags[j].str))){
                codec_id= ff_mkv_codec_tags[j].id;
                break;
            }
        }

        st = track->stream = avformat_new_stream(s, NULL);
        if (st == NULL)
            return AVERROR(ENOMEM);

        if (!strcmp(track->codec_id, "V_MS/VFW/FOURCC")
            && track->codec_priv.size >= 40
            && track->codec_priv.data != NULL) {
            track->ms_compat = 1;
            fourcc = AV_RL32(track->codec_priv.data + 16);
            codec_id = ff_codec_get_id(ff_codec_bmp_tags, fourcc);
            extradata_offset = 40;
        } else if (!strcmp(track->codec_id, "A_MS/ACM")
                   && track->codec_priv.size >= 14
                   && track->codec_priv.data != NULL) {
            int ret;
            ffio_init_context(&b, track->codec_priv.data, track->codec_priv.size,
                              0, NULL, NULL, NULL, NULL);
            ret = ff_get_wav_header(&b, st->codec, track->codec_priv.size);
            if (ret < 0)
                return ret;
            codec_id = st->codec->codec_id;
            extradata_offset = FFMIN(track->codec_priv.size, 18);
        } else if (!strcmp(track->codec_id, "V_QUICKTIME")
                   && (track->codec_priv.size >= 86)
                   && (track->codec_priv.data != NULL)) {
            fourcc = AV_RL32(track->codec_priv.data);
            codec_id = ff_codec_get_id(ff_codec_movvideo_tags, fourcc);
        } else if (codec_id == AV_CODEC_ID_ALAC && track->codec_priv.size && track->codec_priv.size < INT_MAX-12) {
            /
            extradata_size = 12 + track->codec_priv.size;
            extradata = av_mallocz(extradata_size + FF_INPUT_BUFFER_PADDING_SIZE);
            if (extradata == NULL)
                return AVERROR(ENOMEM);
            AV_WB32(extradata, extradata_size);
            memcpy(&extradata[4], "alac", 4);
            AV_WB32(&extradata[8], 0);
            memcpy(&extradata[12], track->codec_priv.data, track->codec_priv.size);
        } else if (codec_id == AV_CODEC_ID_PCM_S16BE) {
            switch (track->audio.bitdepth) {
            case  8:  codec_id = AV_CODEC_ID_PCM_U8;     break;
            case 24:  codec_id = AV_CODEC_ID_PCM_S24BE;  break;
            case 32:  codec_id = AV_CODEC_ID_PCM_S32BE;  break;
            }
        } else if (codec_id == AV_CODEC_ID_PCM_S16LE) {
            switch (track->audio.bitdepth) {
            case  8:  codec_id = AV_CODEC_ID_PCM_U8;     break;
            case 24:  codec_id = AV_CODEC_ID_PCM_S24LE;  break;
            case 32:  codec_id = AV_CODEC_ID_PCM_S32LE;  break;
            }
        } else if (codec_id==AV_CODEC_ID_PCM_F32LE && track->audio.bitdepth==64) {
            codec_id = AV_CODEC_ID_PCM_F64LE;
        } else if (codec_id == AV_CODEC_ID_AAC && !track->codec_priv.size) {
            int profile = matroska_aac_profile(track->codec_id);
            int sri = matroska_aac_sri(track->audio.samplerate);
            extradata = av_mallocz(5 + FF_INPUT_BUFFER_PADDING_SIZE);
            if (extradata == NULL)
                return AVERROR(ENOMEM);
            extradata[0] = (profile << 3) | ((sri&0x0E) >> 1);
            extradata[1] = ((sri&0x01) << 7) | (track->audio.channels<<3);
            if (strstr(track->codec_id, "SBR")) {
                sri = matroska_aac_sri(track->audio.out_samplerate);
                extradata[2] = 0x56;
                extradata[3] = 0xE5;
                extradata[4] = 0x80 | (sri<<3);
                extradata_size = 5;
            } else
                extradata_size = 2;
        } else if (codec_id == AV_CODEC_ID_TTA) {
            extradata_size = 30;
            extradata = av_mallocz(extradata_size + FF_INPUT_BUFFER_PADDING_SIZE);
            if (extradata == NULL)
                return AVERROR(ENOMEM);
            ffio_init_context(&b, extradata, extradata_size, 1,
                          NULL, NULL, NULL, NULL);
            avio_write(&b, "TTA1", 4);
            avio_wl16(&b, 1);
            avio_wl16(&b, track->audio.channels);
            avio_wl16(&b, track->audio.bitdepth);
            avio_wl32(&b, track->audio.out_samplerate);
            avio_wl32(&b, matroska->ctx->duration * track->audio.out_samplerate);
        } else if (codec_id == AV_CODEC_ID_RV10 || codec_id == AV_CODEC_ID_RV20 ||
                   codec_id == AV_CODEC_ID_RV30 || codec_id == AV_CODEC_ID_RV40) {
            extradata_offset = 26;
        } else if (codec_id == AV_CODEC_ID_RA_144) {
            track->audio.out_samplerate = 8000;
            track->audio.channels = 1;
        } else if ((codec_id == AV_CODEC_ID_RA_288 || codec_id == AV_CODEC_ID_COOK ||
                    codec_id == AV_CODEC_ID_ATRAC3 || codec_id == AV_CODEC_ID_SIPR)
                    && track->codec_priv.data) {
            int flavor;

            ffio_init_context(&b, track->codec_priv.data,track->codec_priv.size,
                          0, NULL, NULL, NULL, NULL);
            avio_skip(&b, 22);
            flavor                       = avio_rb16(&b);
            track->audio.coded_framesize = avio_rb32(&b);
            avio_skip(&b, 12);
            track->audio.sub_packet_h    = avio_rb16(&b);
            track->audio.frame_size      = avio_rb16(&b);
            track->audio.sub_packet_size = avio_rb16(&b);
            track->audio.buf = av_malloc(track->audio.frame_size * track->audio.sub_packet_h);
            if (codec_id == AV_CODEC_ID_RA_288) {
                st->codec->block_align = track->audio.coded_framesize;
                track->codec_priv.size = 0;
            } else {
                if (codec_id == AV_CODEC_ID_SIPR && flavor < 4) {
                    const int sipr_bit_rate[4] = { 6504, 8496, 5000, 16000 };
                    track->audio.sub_packet_size = ff_sipr_subpk_size[flavor];
                    st->codec->bit_rate = sipr_bit_rate[flavor];
                }
                st->codec->block_align = track->audio.sub_packet_size;
                extradata_offset = 78;
            }
        }
        track->codec_priv.size -= extradata_offset;

        if (codec_id == AV_CODEC_ID_NONE)
            av_log(matroska->ctx, AV_LOG_INFO,
                   "Unknown/unsupported AVCodecID %s.\n", track->codec_id);

        if (track->time_scale < 0.01)
            track->time_scale = 1.0;
        avpriv_set_pts_info(st, 64, matroska->time_scale*track->time_scale, 1000*1000*1000); /

        st->codec->codec_id = codec_id;
        st->start_time = 0;
        if (strcmp(track->language, "und"))
            av_dict_set(&st->metadata, "language", track->language, 0);
        av_dict_set(&st->metadata, "title", track->name, 0);

        if (track->flag_default)
            st->disposition |= AV_DISPOSITION_DEFAULT;
        if (track->flag_forced)
            st->disposition |= AV_DISPOSITION_FORCED;

        if (!st->codec->extradata) {
            if(extradata){
                st->codec->extradata = extradata;
                st->codec->extradata_size = extradata_size;
            } else if(track->codec_priv.data && track->codec_priv.size > 0){
                st->codec->extradata = av_mallocz(track->codec_priv.size +
                                                  FF_INPUT_BUFFER_PADDING_SIZE);
                if(st->codec->extradata == NULL)
                    return AVERROR(ENOMEM);
                st->codec->extradata_size = track->codec_priv.size;
                memcpy(st->codec->extradata,
                       track->codec_priv.data + extradata_offset,
                       track->codec_priv.size);
            }
        }

        if (track->type == MATROSKA_TRACK_TYPE_VIDEO) {
            MatroskaTrackPlane *planes = track->operation.combine_planes.elem;

            st->codec->codec_type = AVMEDIA_TYPE_VIDEO;
            st->codec->codec_tag  = fourcc;
            st->codec->width  = track->video.pixel_width;
            st->codec->height = track->video.pixel_height;
            av_reduce(&st->sample_aspect_ratio.num,
                      &st->sample_aspect_ratio.den,
                      st->codec->height * track->video.display_width,
                      st->codec-> width * track->video.display_height,
                      255);
            st->need_parsing = AVSTREAM_PARSE_HEADERS;
            if (track->default_duration) {
                av_reduce(&st->avg_frame_rate.num, &st->avg_frame_rate.den,
                          1000000000, track->default_duration, 30000);
#if FF_API_R_FRAME_RATE
                st->r_frame_rate = st->avg_frame_rate;
#endif
            }

            /
            if (track->video.stereo_mode && track->video.stereo_mode < MATROSKA_VIDEO_STEREO_MODE_COUNT)
                av_dict_set(&st->metadata, "stereo_mode", ff_matroska_video_stereo_mode[track->video.stereo_mode], 0);

            /
            if (track->video.alpha_mode)
                av_dict_set(&st->metadata, "alpha_mode", "1", 0);

            /
            for (j=0; j < track->operation.combine_planes.nb_elem; j++) {
                char buf[32];
                if (planes[j].type >= MATROSKA_VIDEO_STEREO_PLANE_COUNT)
                    continue;
                snprintf(buf, sizeof(buf), "%s_%d",
                         ff_matroska_video_stereo_plane[planes[j].type], i);
                for (k=0; k < matroska->tracks.nb_elem; k++)
                    if (planes[j].uid == tracks[k].uid) {
                        av_dict_set(&s->streams[k]->metadata,
                                    "stereo_mode", buf, 0);
                        break;
                    }
            }
        } else if (track->type == MATROSKA_TRACK_TYPE_AUDIO) {
            st->codec->codec_type = AVMEDIA_TYPE_AUDIO;
            st->codec->sample_rate = track->audio.out_samplerate;
            st->codec->channels = track->audio.channels;
            st->codec->bits_per_coded_sample = track->audio.bitdepth;
            if (st->codec->codec_id != AV_CODEC_ID_AAC)
            st->need_parsing = AVSTREAM_PARSE_HEADERS;
        } else if (track->type == MATROSKA_TRACK_TYPE_SUBTITLE) {
            st->codec->codec_type = AVMEDIA_TYPE_SUBTITLE;
            if (st->codec->codec_id == AV_CODEC_ID_SSA)
                matroska->contains_ssa = 1;
        }
    }

    attachements = attachements_list->elem;
    for (j=0; j<attachements_list->nb_elem; j++) {
        if (!(attachements[j].filename && attachements[j].mime &&
              attachements[j].bin.data && attachements[j].bin.size > 0)) {
            av_log(matroska->ctx, AV_LOG_ERROR, "incomplete attachment\n");
        } else {
            AVStream *st = avformat_new_stream(s, NULL);
            if (st == NULL)
                break;
            av_dict_set(&st->metadata, "filename",attachements[j].filename, 0);
            av_dict_set(&st->metadata, "mimetype", attachements[j].mime, 0);
            st->codec->codec_id = AV_CODEC_ID_NONE;
            st->codec->codec_type = AVMEDIA_TYPE_ATTACHMENT;
            st->codec->extradata  = av_malloc(attachements[j].bin.size + FF_INPUT_BUFFER_PADDING_SIZE);
            if(st->codec->extradata == NULL)
                break;
            st->codec->extradata_size = attachements[j].bin.size;
            memcpy(st->codec->extradata, attachements[j].bin.data, attachements[j].bin.size);

            for (i=0; ff_mkv_mime_tags[i].id != AV_CODEC_ID_NONE; i++) {
                if (!strncmp(ff_mkv_mime_tags[i].str, attachements[j].mime,
                             strlen(ff_mkv_mime_tags[i].str))) {
                    st->codec->codec_id = ff_mkv_mime_tags[i].id;
                    break;
                }
            }
            attachements[j].stream = st;
        }
    }

    chapters = chapters_list->elem;
    for (i=0; i<chapters_list->nb_elem; i++)
        if (chapters[i].start != AV_NOPTS_VALUE && chapters[i].uid
            && (max_start==0 || chapters[i].start > max_start)) {
            chapters[i].chapter =
            avpriv_new_chapter(s, chapters[i].uid, (AVRational){1, 1000000000},
                           chapters[i].start, chapters[i].end,
                           chapters[i].title);
            av_dict_set(&chapters[i].chapter->metadata,
                             "title", chapters[i].title, 0);
            max_start = chapters[i].start;
        }

    matroska_add_index_entries(matroska);

    matroska_convert_tags(s);

    return 0;
}
