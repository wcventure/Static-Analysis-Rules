static int
matroska_read_header (AVFormatContext    *s,
                      AVFormatParameters *ap)
{
    MatroskaDemuxContext *matroska = s->priv_data;
    char *doctype;
    int version, last_level, res = 0;
    uint32_t id;

    matroska->ctx = s;

    /
    doctype = NULL;
    if ((res = ebml_read_header(matroska, &doctype, &version)) < 0)
        return res;
    if ((doctype == NULL) || strcmp(doctype, "matroska")) {
        av_log(matroska->ctx, AV_LOG_ERROR,
               "Wrong EBML doctype ('%s' != 'matroska').\n",
               doctype ? doctype : "(none)");
        if (doctype)
            av_free(doctype);
        return AVERROR_NOFMT;
    }
    av_free(doctype);
    if (version > 2) {
        av_log(matroska->ctx, AV_LOG_ERROR,
               "Matroska demuxer version 2 too old for file version %d\n",
               version);
        return AVERROR_NOFMT;
    }

    /
    while (1) {
        if (!(id = ebml_peek_id(matroska, &last_level)))
            return AVERROR(EIO);
        if (id == MATROSKA_ID_SEGMENT)
            break;

        /
        av_log(matroska->ctx, AV_LOG_INFO,
               "Expected a Segment ID (0x%x), but received 0x%x!\n",
               MATROSKA_ID_SEGMENT, id);
        if ((res = ebml_read_skip(matroska)) < 0)
            return res;
    }

    /
    if ((res = ebml_read_master(matroska, &id)) < 0)
        return res;
    matroska->segment_start = url_ftell(s->pb);

    matroska->time_scale = 1000000;
    /
    while (res == 0) {
        if (!(id = ebml_peek_id(matroska, &matroska->level_up))) {
            res = AVERROR(EIO);
            break;
        } else if (matroska->level_up) {
            matroska->level_up--;
            break;
        }

        switch (id) {
            /
            case MATROSKA_ID_INFO: {
                if ((res = ebml_read_master(matroska, &id)) < 0)
                    break;
                res = matroska_parse_info(matroska);
                break;
            }

            /
            case MATROSKA_ID_TRACKS: {
                if ((res = ebml_read_master(matroska, &id)) < 0)
                    break;
                res = matroska_parse_tracks(matroska);
                break;
            }

            /
            case MATROSKA_ID_CUES: {
                if (!matroska->index_parsed) {
                    if ((res = ebml_read_master(matroska, &id)) < 0)
                        break;
                    res = matroska_parse_index(matroska);
                } else
                    res = ebml_read_skip(matroska);
                break;
            }

            /
            case MATROSKA_ID_TAGS: {
                if (!matroska->metadata_parsed) {
                    if ((res = ebml_read_master(matroska, &id)) < 0)
                        break;
                    res = matroska_parse_metadata(matroska);
                } else
                    res = ebml_read_skip(matroska);
                break;
            }

            /
            case MATROSKA_ID_SEEKHEAD: {
                if ((res = ebml_read_master(matroska, &id)) < 0)
                    break;
                res = matroska_parse_seekhead(matroska);
                break;
            }

            case MATROSKA_ID_ATTACHMENTS: {
                if ((res = ebml_read_master(matroska, &id)) < 0)
                    break;
                res = matroska_parse_attachments(s);
                break;
            }

            case MATROSKA_ID_CLUSTER: {
                /
                res = 1;
                break;
            }

            default:
                av_log(matroska->ctx, AV_LOG_INFO,
                       "Unknown matroska file header ID 0x%x\n", id);
            /

            case EBML_ID_VOID:
                res = ebml_read_skip(matroska);
                break;
        }

        if (matroska->level_up) {
            matroska->level_up--;
            break;
        }
    }

    /
    if (ebml_peek_id(matroska, NULL) == MATROSKA_ID_CLUSTER) {
        int i, j;
        MatroskaTrack *track;
        AVStream *st;

        for (i = 0; i < matroska->num_tracks; i++) {
            enum CodecID codec_id = CODEC_ID_NONE;
            uint8_t *extradata = NULL;
            int extradata_size = 0;
            int extradata_offset = 0;
            track = matroska->tracks[i];
            track->stream_index = -1;

            /
            if (track->codec_id == NULL)
                continue;

            for(j=0; ff_mkv_codec_tags[j].id != CODEC_ID_NONE; j++){
                if(!strncmp(ff_mkv_codec_tags[j].str, track->codec_id,
                            strlen(ff_mkv_codec_tags[j].str))){
                    codec_id= ff_mkv_codec_tags[j].id;
                    break;
                }
            }

            /
            /
            if (!strcmp(track->codec_id,
                        MATROSKA_CODEC_ID_VIDEO_VFW_FOURCC) &&
                (track->codec_priv_size >= 40) &&
                (track->codec_priv != NULL)) {
                MatroskaVideoTrack *vtrack = (MatroskaVideoTrack *) track;

                /
                vtrack->fourcc = AV_RL32(track->codec_priv + 16);
                codec_id = codec_get_id(codec_bmp_tags, vtrack->fourcc);

            }

            /
            else if (!strcmp(track->codec_id,
                             MATROSKA_CODEC_ID_AUDIO_ACM) &&
                (track->codec_priv_size >= 18) &&
                (track->codec_priv != NULL)) {
                uint16_t tag;

                /
                tag = AV_RL16(track->codec_priv);
                codec_id = codec_get_id(codec_wav_tags, tag);

            }

            else if (codec_id == CODEC_ID_AAC && !track->codec_priv_size) {
                MatroskaAudioTrack *audiotrack = (MatroskaAudioTrack *) track;
                int profile = matroska_aac_profile(track->codec_id);
                int sri = matroska_aac_sri(audiotrack->internal_samplerate);
                extradata = av_malloc(5);
                if (extradata == NULL)
                    return AVERROR(ENOMEM);
                extradata[0] = (profile << 3) | ((sri&0x0E) >> 1);
                extradata[1] = ((sri&0x01) << 7) | (audiotrack->channels<<3);
                if (strstr(track->codec_id, "SBR")) {
                    sri = matroska_aac_sri(audiotrack->samplerate);
                    extradata[2] = 0x56;
                    extradata[3] = 0xE5;
                    extradata[4] = 0x80 | (sri<<3);
                    extradata_size = 5;
                } else {
                    extradata_size = 2;
                }
            }

            else if (codec_id == CODEC_ID_TTA) {
                MatroskaAudioTrack *audiotrack = (MatroskaAudioTrack *) track;
                ByteIOContext b;
                extradata_size = 30;
                extradata = av_mallocz(extradata_size);
                if (extradata == NULL)
                    return AVERROR(ENOMEM);
                init_put_byte(&b, extradata, extradata_size, 1,
                              NULL, NULL, NULL, NULL);
                put_buffer(&b, "TTA1", 4);
                put_le16(&b, 1);
                put_le16(&b, audiotrack->channels);
                put_le16(&b, audiotrack->bitdepth);
                put_le32(&b, audiotrack->samplerate);
                put_le32(&b, matroska->ctx->duration * audiotrack->samplerate);
            }

            else if (codec_id == CODEC_ID_RV10 || codec_id == CODEC_ID_RV20 ||
                     codec_id == CODEC_ID_RV30 || codec_id == CODEC_ID_RV40) {
                extradata_offset = 26;
                track->codec_priv_size -= extradata_offset;
            }

            else if (codec_id == CODEC_ID_RA_144) {
                MatroskaAudioTrack *audiotrack = (MatroskaAudioTrack *)track;
                audiotrack->samplerate = 8000;
                audiotrack->channels = 1;
            }

            else if (codec_id == CODEC_ID_RA_288 ||
                     codec_id == CODEC_ID_COOK ||
                     codec_id == CODEC_ID_ATRAC3) {
                MatroskaAudioTrack *audiotrack = (MatroskaAudioTrack *)track;
                ByteIOContext b;

                init_put_byte(&b, track->codec_priv, track->codec_priv_size, 0,
                              NULL, NULL, NULL, NULL);
                url_fskip(&b, 24);
                audiotrack->coded_framesize = get_be32(&b);
                url_fskip(&b, 12);
                audiotrack->sub_packet_h    = get_be16(&b);
                audiotrack->frame_size      = get_be16(&b);
                audiotrack->sub_packet_size = get_be16(&b);
                audiotrack->buf = av_malloc(audiotrack->frame_size * audiotrack->sub_packet_h);
                if (codec_id == CODEC_ID_RA_288) {
                    audiotrack->block_align = audiotrack->coded_framesize;
                    track->codec_priv_size = 0;
                } else {
                    audiotrack->block_align = audiotrack->sub_packet_size;
                    extradata_offset = 78;
                    track->codec_priv_size -= extradata_offset;
                }
            }

            if (codec_id == CODEC_ID_NONE) {
                av_log(matroska->ctx, AV_LOG_INFO,
                       "Unknown/unsupported CodecID %s.\n",
                       track->codec_id);
            }

            track->stream_index = matroska->num_streams;

            matroska->num_streams++;
            st = av_new_stream(s, track->stream_index);
            if (st == NULL)
                return AVERROR(ENOMEM);
            av_set_pts_info(st, 64, matroska->time_scale, 1000*1000*1000); /

            st->codec->codec_id = codec_id;
            st->start_time = 0;
            if (strcmp(track->language, "und"))
                strcpy(st->language, track->language);

            if (track->flags & MATROSKA_TRACK_DEFAULT)
                st->disposition |= AV_DISPOSITION_DEFAULT;

            if (track->default_duration)
                av_reduce(&st->codec->time_base.num, &st->codec->time_base.den,
                          track->default_duration, 1000000000, 30000);

            if(extradata){
                st->codec->extradata = extradata;
                st->codec->extradata_size = extradata_size;
            } else if(track->codec_priv && track->codec_priv_size > 0){
                st->codec->extradata = av_malloc(track->codec_priv_size);
                if(st->codec->extradata == NULL)
                    return AVERROR(ENOMEM);
                st->codec->extradata_size = track->codec_priv_size;
                memcpy(st->codec->extradata,track->codec_priv+extradata_offset,
                       track->codec_priv_size);
            }

            if (track->type == MATROSKA_TRACK_TYPE_VIDEO) {
                MatroskaVideoTrack *videotrack = (MatroskaVideoTrack *)track;

                st->codec->codec_type = CODEC_TYPE_VIDEO;
                st->codec->codec_tag = videotrack->fourcc;
                st->codec->width = videotrack->pixel_width;
                st->codec->height = videotrack->pixel_height;
                if (videotrack->display_width == 0)
                    videotrack->display_width= videotrack->pixel_width;
                if (videotrack->display_height == 0)
                    videotrack->display_height= videotrack->pixel_height;
                av_reduce(&st->codec->sample_aspect_ratio.num,
                          &st->codec->sample_aspect_ratio.den,
                          st->codec->height * videotrack->display_width,
                          st->codec-> width * videotrack->display_height,
                          255);
                st->need_parsing = AVSTREAM_PARSE_HEADERS;
            } else if (track->type == MATROSKA_TRACK_TYPE_AUDIO) {
                MatroskaAudioTrack *audiotrack = (MatroskaAudioTrack *)track;

                st->codec->codec_type = CODEC_TYPE_AUDIO;
                st->codec->sample_rate = audiotrack->samplerate;
                st->codec->channels = audiotrack->channels;
                st->codec->block_align = audiotrack->block_align;
            } else if (track->type == MATROSKA_TRACK_TYPE_SUBTITLE) {
                st->codec->codec_type = CODEC_TYPE_SUBTITLE;
            }

            /
        }
        res = 0;
    }

    if (matroska->index_parsed) {
        int i, track, stream;
        for (i=0; i<matroska->num_indexes; i++) {
            MatroskaDemuxIndex *idx = &matroska->index[i];
            track = matroska_find_track_by_num(matroska, idx->track);
            stream = matroska->tracks[track]->stream_index;
            if (stream >= 0)
                av_add_index_entry(matroska->ctx->streams[stream],
                                   idx->pos, idx->time/matroska->time_scale,
                                   0, 0, AVINDEX_KEYFRAME);
        }
    }

    return res;
}
