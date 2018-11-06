static int matroska_parse_tracks(AVFormatContext *s)
{
    MatroskaDemuxContext *matroska = s->priv_data;
    MatroskaTrack *tracks = matroska->tracks.elem;
    AVStream *st;
    int i, j, ret;
    int k;

    for (i = 0; i < matroska->tracks.nb_elem; i++) {
        MatroskaTrack *track = &tracks[i];
        enum AVCodecID codec_id = AV_CODEC_ID_NONE;
        EbmlList *encodings_list = &track->encodings;
        MatroskaTrackEncoding *encodings = encodings_list->elem;
        uint8_t *extradata = NULL;
        int extradata_size = 0;
        int extradata_offset = 0;
        uint32_t fourcc = 0;
        AVIOContext b;
        char* key_id_base64 = NULL;
        int bit_depth = -1;

        /
        if (track->type != MATROSKA_TRACK_TYPE_VIDEO &&
            track->type != MATROSKA_TRACK_TYPE_AUDIO &&
            track->type != MATROSKA_TRACK_TYPE_SUBTITLE &&
            track->type != MATROSKA_TRACK_TYPE_METADATA) {
            av_log(matroska->ctx, AV_LOG_INFO,
                   "Unknown or unsupported track type %"PRIu64"\n",
                   track->type);
            continue;
        }
        if (!track->codec_id)
            continue;

        if (track->audio.samplerate < 0 || track->audio.samplerate > INT_MAX ||
            isnan(track->audio.samplerate)) {
            av_log(matroska->ctx, AV_LOG_WARNING,
                   "Invalid sample rate %f, defaulting to 8000 instead.\n",
                   track->audio.samplerate);
            track->audio.samplerate = 8000;
        }

        if (track->type == MATROSKA_TRACK_TYPE_VIDEO) {
            if (!track->default_duration && track->video.frame_rate > 0)
                track->default_duration = 1000000000 / track->video.frame_rate;
            if (track->video.display_width == -1)
                track->video.display_width = track->video.pixel_width;
            if (track->video.display_height == -1)
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
            if (encodings[0].type) {
                if (encodings[0].encryption.key_id.size > 0) {
                    /
                    const int b64_size = AV_BASE64_SIZE(encodings[0].encryption.key_id.size);
                    key_id_base64 = av_malloc(b64_size);
                    if (key_id_base64 == NULL)
                        return AVERROR(ENOMEM);

                    av_base64_encode(key_id_base64, b64_size,
                                     encodings[0].encryption.key_id.data,
                                     encodings[0].encryption.key_id.size);
                } else {
                    encodings[0].scope = 0;
                    av_log(matroska->ctx, AV_LOG_ERROR,
                           "Unsupported encoding type");
                }
            } else if (
#if CONFIG_ZLIB
                 encodings[0].compression.algo != MATROSKA_TRACK_ENCODING_COMP_ZLIB  &&
#endif
#if CONFIG_BZLIB
                 encodings[0].compression.algo != MATROSKA_TRACK_ENCODING_COMP_BZLIB &&
#endif
#if CONFIG_LZO
                 encodings[0].compression.algo != MATROSKA_TRACK_ENCODING_COMP_LZO   &&
#endif
                 encodings[0].compression.algo != MATROSKA_TRACK_ENCODING_COMP_HEADERSTRIP) {
                encodings[0].scope = 0;
                av_log(matroska->ctx, AV_LOG_ERROR,
                       "Unsupported encoding type");
            } else if (track->codec_priv.size && encodings[0].scope & 2) {
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

        for (j = 0; ff_mkv_codec_tags[j].id != AV_CODEC_ID_NONE; j++) {
            if (!strncmp(ff_mkv_codec_tags[j].str, track->codec_id,
                         strlen(ff_mkv_codec_tags[j].str))) {
                codec_id = ff_mkv_codec_tags[j].id;
                break;
            }
        }

        st = track->stream = avformat_new_stream(s, NULL);
        if (!st) {
            av_free(key_id_base64);
            return AVERROR(ENOMEM);
        }

        if (key_id_base64) {
            /
            av_dict_set(&st->metadata, "enc_key_id", key_id_base64, 0);
            av_freep(&key_id_base64);
        }

        if (!strcmp(track->codec_id, "V_MS/VFW/FOURCC") &&
             track->codec_priv.size >= 40               &&
            track->codec_priv.data) {
            track->ms_compat    = 1;
            bit_depth           = AV_RL16(track->codec_priv.data + 14);
            fourcc              = AV_RL32(track->codec_priv.data + 16);
            codec_id            = ff_codec_get_id(ff_codec_bmp_tags,
                                                  fourcc);
            if (!codec_id)
                codec_id        = ff_codec_get_id(ff_codec_movvideo_tags,
                                                  fourcc);
            extradata_offset    = 40;
        } else if (!strcmp(track->codec_id, "A_MS/ACM") &&
                   track->codec_priv.size >= 14         &&
                   track->codec_priv.data) {
            int ret;
            ffio_init_context(&b, track->codec_priv.data,
                              track->codec_priv.size,
                              0, NULL, NULL, NULL, NULL);
            ret = ff_get_wav_header(s, &b, st->codecpar, track->codec_priv.size, 0);
            if (ret < 0)
                return ret;
            codec_id         = st->codecpar->codec_id;
            fourcc           = st->codecpar->codec_tag;
            extradata_offset = FFMIN(track->codec_priv.size, 18);
        } else if (!strcmp(track->codec_id, "A_QUICKTIME")
                   /
                   && (track->codec_priv.size >= 32)
                   && (track->codec_priv.data)) {
            uint16_t sample_size;
            int ret = get_qt_codec(track, &fourcc, &codec_id);
            if (ret < 0)
                return ret;
            sample_size = AV_RB16(track->codec_priv.data + 26);
            if (fourcc == 0) {
                if (sample_size == 8) {
                    fourcc = MKTAG('r','a','w',' ');
                    codec_id = ff_codec_get_id(ff_codec_movaudio_tags, fourcc);
                } else if (sample_size == 16) {
                    fourcc = MKTAG('t','w','o','s');
                    codec_id = ff_codec_get_id(ff_codec_movaudio_tags, fourcc);
                }
            }
            if ((fourcc == MKTAG('t','w','o','s') ||
                    fourcc == MKTAG('s','o','w','t')) &&
                    sample_size == 8)
                codec_id = AV_CODEC_ID_PCM_S8;
        } else if (!strcmp(track->codec_id, "V_QUICKTIME") &&
                   (track->codec_priv.size >= 21)          &&
                   (track->codec_priv.data)) {
            int ret = get_qt_codec(track, &fourcc, &codec_id);
            if (ret < 0)
                return ret;
            if (codec_id == AV_CODEC_ID_NONE && AV_RL32(track->codec_priv.data+4) == AV_RL32("SMI ")) {
                fourcc = MKTAG('S','V','Q','3');
                codec_id = ff_codec_get_id(ff_codec_movvideo_tags, fourcc);
            }
            if (codec_id == AV_CODEC_ID_NONE)
                av_log(matroska->ctx, AV_LOG_ERROR,
                       "mov FourCC not found %s.\n", av_fourcc2str(fourcc));
            if (track->codec_priv.size >= 86) {
                bit_depth = AV_RB16(track->codec_priv.data + 82);
                ffio_init_context(&b, track->codec_priv.data,
                                  track->codec_priv.size,
                                  0, NULL, NULL, NULL, NULL);
                if (ff_get_qtpalette(codec_id, &b, track->palette)) {
                    bit_depth &= 0x1F;
                    track->has_palette = 1;
                }
            }
        } else if (codec_id == AV_CODEC_ID_PCM_S16BE) {
            switch (track->audio.bitdepth) {
            case  8:
                codec_id = AV_CODEC_ID_PCM_U8;
                break;
            case 24:
                codec_id = AV_CODEC_ID_PCM_S24BE;
                break;
            case 32:
                codec_id = AV_CODEC_ID_PCM_S32BE;
                break;
            }
        } else if (codec_id == AV_CODEC_ID_PCM_S16LE) {
            switch (track->audio.bitdepth) {
            case  8:
                codec_id = AV_CODEC_ID_PCM_U8;
                break;
            case 24:
                codec_id = AV_CODEC_ID_PCM_S24LE;
                break;
            case 32:
                codec_id = AV_CODEC_ID_PCM_S32LE;
                break;
            }
        } else if (codec_id == AV_CODEC_ID_PCM_F32LE &&
                   track->audio.bitdepth == 64) {
            codec_id = AV_CODEC_ID_PCM_F64LE;
        } else if (codec_id == AV_CODEC_ID_AAC && !track->codec_priv.size) {
            int profile = matroska_aac_profile(track->codec_id);
            int sri     = matroska_aac_sri(track->audio.samplerate);
            extradata   = av_mallocz(5 + AV_INPUT_BUFFER_PADDING_SIZE);
            if (!extradata)
                return AVERROR(ENOMEM);
            extradata[0] = (profile << 3) | ((sri & 0x0E) >> 1);
            extradata[1] = ((sri & 0x01) << 7) | (track->audio.channels << 3);
            if (strstr(track->codec_id, "SBR")) {
                sri            = matroska_aac_sri(track->audio.out_samplerate);
                extradata[2]   = 0x56;
                extradata[3]   = 0xE5;
                extradata[4]   = 0x80 | (sri << 3);
                extradata_size = 5;
            } else
                extradata_size = 2;
        } else if (codec_id == AV_CODEC_ID_ALAC && track->codec_priv.size && track->codec_priv.size < INT_MAX - 12 - AV_INPUT_BUFFER_PADDING_SIZE) {
            /
            extradata_size = 12 + track->codec_priv.size;
            extradata      = av_mallocz(extradata_size +
                                        AV_INPUT_BUFFER_PADDING_SIZE);
            if (!extradata)
                return AVERROR(ENOMEM);
            AV_WB32(extradata, extradata_size);
            memcpy(&extradata[4], "alac", 4);
            AV_WB32(&extradata[8], 0);
            memcpy(&extradata[12], track->codec_priv.data,
                   track->codec_priv.size);
        } else if (codec_id == AV_CODEC_ID_TTA) {
            extradata_size = 30;
            extradata      = av_mallocz(extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
            if (!extradata)
                return AVERROR(ENOMEM);
            ffio_init_context(&b, extradata, extradata_size, 1,
                              NULL, NULL, NULL, NULL);
            avio_write(&b, "TTA1", 4);
            avio_wl16(&b, 1);
            if (track->audio.channels > UINT16_MAX ||
                track->audio.bitdepth > UINT16_MAX) {
                av_log(matroska->ctx, AV_LOG_WARNING,
                       "Too large audio channel number %"PRIu64
                       " or bitdepth %"PRIu64". Skipping track.\n",
                       track->audio.channels, track->audio.bitdepth);
                av_freep(&extradata);
                if (matroska->ctx->error_recognition & AV_EF_EXPLODE)
                    return AVERROR_INVALIDDATA;
                else
                    continue;
            }
            avio_wl16(&b, track->audio.channels);
            avio_wl16(&b, track->audio.bitdepth);
            if (track->audio.out_samplerate < 0 || track->audio.out_samplerate > INT_MAX)
                return AVERROR_INVALIDDATA;
            avio_wl32(&b, track->audio.out_samplerate);
            avio_wl32(&b, av_rescale((matroska->duration * matroska->time_scale),
                                     track->audio.out_samplerate,
                                     AV_TIME_BASE * 1000));
        } else if (codec_id == AV_CODEC_ID_RV10 ||
                   codec_id == AV_CODEC_ID_RV20 ||
                   codec_id == AV_CODEC_ID_RV30 ||
                   codec_id == AV_CODEC_ID_RV40) {
            extradata_offset = 26;
        } else if (codec_id == AV_CODEC_ID_RA_144) {
            track->audio.out_samplerate = 8000;
            track->audio.channels       = 1;
        } else if ((codec_id == AV_CODEC_ID_RA_288 ||
                    codec_id == AV_CODEC_ID_COOK   ||
                    codec_id == AV_CODEC_ID_ATRAC3 ||
                    codec_id == AV_CODEC_ID_SIPR)
                      && track->codec_priv.data) {
            int flavor;

            ffio_init_context(&b, track->codec_priv.data,
                              track->codec_priv.size,
                              0, NULL, NULL, NULL, NULL);
            avio_skip(&b, 22);
            flavor                       = avio_rb16(&b);
            track->audio.coded_framesize = avio_rb32(&b);
            avio_skip(&b, 12);
            track->audio.sub_packet_h    = avio_rb16(&b);
            track->audio.frame_size      = avio_rb16(&b);
            track->audio.sub_packet_size = avio_rb16(&b);
            if (flavor                        < 0 ||
                track->audio.coded_framesize <= 0 ||
                track->audio.sub_packet_h    <= 0 ||
                track->audio.frame_size      <= 0 ||
                track->audio.sub_packet_size <= 0 && codec_id != AV_CODEC_ID_SIPR)
                return AVERROR_INVALIDDATA;
            track->audio.buf = av_malloc_array(track->audio.sub_packet_h,
                                               track->audio.frame_size);
            if (!track->audio.buf)
                return AVERROR(ENOMEM);
            if (codec_id == AV_CODEC_ID_RA_288) {
                st->codecpar->block_align = track->audio.coded_framesize;
                track->codec_priv.size = 0;
            } else {
                if (codec_id == AV_CODEC_ID_SIPR && flavor < 4) {
                    static const int sipr_bit_rate[4] = { 6504, 8496, 5000, 16000 };
                    track->audio.sub_packet_size = ff_sipr_subpk_size[flavor];
                    st->codecpar->bit_rate          = sipr_bit_rate[flavor];
                }
                st->codecpar->block_align = track->audio.sub_packet_size;
                extradata_offset       = 78;
            }
        } else if (codec_id == AV_CODEC_ID_FLAC && track->codec_priv.size) {
            ret = matroska_parse_flac(s, track, &extradata_offset);
            if (ret < 0)
                return ret;
        } else if (codec_id == AV_CODEC_ID_PRORES && track->codec_priv.size == 4) {
            fourcc = AV_RL32(track->codec_priv.data);
        }
        track->codec_priv.size -= extradata_offset;

        if (codec_id == AV_CODEC_ID_NONE)
            av_log(matroska->ctx, AV_LOG_INFO,
                   "Unknown/unsupported AVCodecID %s.\n", track->codec_id);

        if (track->time_scale < 0.01)
            track->time_scale = 1.0;
        avpriv_set_pts_info(st, 64, matroska->time_scale * track->time_scale,
                            1000 * 1000 * 1000);    /

        /
        track->codec_delay_in_track_tb = av_rescale_q(track->codec_delay,
                                          (AVRational){ 1, 1000000000 },
                                          st->time_base);

        st->codecpar->codec_id = codec_id;

        if (strcmp(track->language, "und"))
            av_dict_set(&st->metadata, "language", track->language, 0);
        av_dict_set(&st->metadata, "title", track->name, 0);

        if (track->flag_default)
            st->disposition |= AV_DISPOSITION_DEFAULT;
        if (track->flag_forced)
            st->disposition |= AV_DISPOSITION_FORCED;

        if (!st->codecpar->extradata) {
            if (extradata) {
                st->codecpar->extradata      = extradata;
                st->codecpar->extradata_size = extradata_size;
            } else if (track->codec_priv.data && track->codec_priv.size > 0) {
                if (ff_alloc_extradata(st->codecpar, track->codec_priv.size))
                    return AVERROR(ENOMEM);
                memcpy(st->codecpar->extradata,
                       track->codec_priv.data + extradata_offset,
                       track->codec_priv.size);
            }
        }

        if (track->type == MATROSKA_TRACK_TYPE_VIDEO) {
            MatroskaTrackPlane *planes = track->operation.combine_planes.elem;
            int display_width_mul  = 1;
            int display_height_mul = 1;

            st->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
            st->codecpar->codec_tag  = fourcc;
            if (bit_depth >= 0)
                st->codecpar->bits_per_coded_sample = bit_depth;
            st->codecpar->width      = track->video.pixel_width;
            st->codecpar->height     = track->video.pixel_height;

            if (track->video.interlaced == MATROSKA_VIDEO_INTERLACE_FLAG_INTERLACED)
                st->codecpar->field_order = mkv_field_order(matroska, track->video.field_order);
            else if (track->video.interlaced == MATROSKA_VIDEO_INTERLACE_FLAG_PROGRESSIVE)
                st->codecpar->field_order = AV_FIELD_PROGRESSIVE;

            if (track->video.stereo_mode && track->video.stereo_mode < MATROSKA_VIDEO_STEREOMODE_TYPE_NB)
                mkv_stereo_mode_display_mul(track->video.stereo_mode, &display_width_mul, &display_height_mul);

            if (track->video.display_unit < MATROSKA_VIDEO_DISPLAYUNIT_UNKNOWN) {
                av_reduce(&st->sample_aspect_ratio.num,
                          &st->sample_aspect_ratio.den,
                          st->codecpar->height * track->video.display_width  * display_width_mul,
                          st->codecpar->width  * track->video.display_height * display_height_mul,
                          255);
            }
            if (st->codecpar->codec_id != AV_CODEC_ID_HEVC)
                st->need_parsing = AVSTREAM_PARSE_HEADERS;

            if (track->default_duration) {
                av_reduce(&st->avg_frame_rate.num, &st->avg_frame_rate.den,
                          1000000000, track->default_duration, 30000);
#if FF_API_R_FRAME_RATE
                if (   st->avg_frame_rate.num < st->avg_frame_rate.den * 1000LL
                    && st->avg_frame_rate.num > st->avg_frame_rate.den * 5LL)
                    st->r_frame_rate = st->avg_frame_rate;
#endif
            }

            /
            if (track->video.stereo_mode && track->video.stereo_mode < MATROSKA_VIDEO_STEREOMODE_TYPE_NB)
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
                    if (planes[j].uid == tracks[k].uid && tracks[k].stream) {
                        av_dict_set(&tracks[k].stream->metadata,
                                    "stereo_mode", buf, 0);
                        break;
                    }
            }
            // add stream level stereo3d side data if it is a supported format
            if (track->video.stereo_mode < MATROSKA_VIDEO_STEREOMODE_TYPE_NB &&
                track->video.stereo_mode != 10 && track->video.stereo_mode != 12) {
                int ret = ff_mkv_stereo3d_conv(st, track->video.stereo_mode);
                if (ret < 0)
                    return ret;
            }

            ret = mkv_parse_video_color(st, track);
            if (ret < 0)
                return ret;
            ret = mkv_parse_video_projection(st, track);
            if (ret < 0)
                return ret;
        } else if (track->type == MATROSKA_TRACK_TYPE_AUDIO) {
            st->codecpar->codec_type  = AVMEDIA_TYPE_AUDIO;
            st->codecpar->codec_tag   = fourcc;
            st->codecpar->sample_rate = track->audio.out_samplerate;
            st->codecpar->channels    = track->audio.channels;
            if (!st->codecpar->bits_per_coded_sample)
                st->codecpar->bits_per_coded_sample = track->audio.bitdepth;
            if (st->codecpar->codec_id == AV_CODEC_ID_MP3)
                st->need_parsing = AVSTREAM_PARSE_FULL;
            else if (st->codecpar->codec_id != AV_CODEC_ID_AAC)
                st->need_parsing = AVSTREAM_PARSE_HEADERS;
            if (track->codec_delay > 0) {
                st->codecpar->initial_padding = av_rescale_q(track->codec_delay,
                                                             (AVRational){1, 1000000000},
                                                             (AVRational){1, st->codecpar->codec_id == AV_CODEC_ID_OPUS ?
                                                                             48000 : st->codecpar->sample_rate});
            }
            if (track->seek_preroll > 0) {
                st->codecpar->seek_preroll = av_rescale_q(track->seek_preroll,
                                                          (AVRational){1, 1000000000},
                                                          (AVRational){1, st->codecpar->sample_rate});
            }
        } else if (codec_id == AV_CODEC_ID_WEBVTT) {
            st->codecpar->codec_type = AVMEDIA_TYPE_SUBTITLE;

            if (!strcmp(track->codec_id, "D_WEBVTT/CAPTIONS")) {
                st->disposition |= AV_DISPOSITION_CAPTIONS;
            } else if (!strcmp(track->codec_id, "D_WEBVTT/DESCRIPTIONS")) {
                st->disposition |= AV_DISPOSITION_DESCRIPTIONS;
            } else if (!strcmp(track->codec_id, "D_WEBVTT/METADATA")) {
                st->disposition |= AV_DISPOSITION_METADATA;
            }
        } else if (track->type == MATROSKA_TRACK_TYPE_SUBTITLE) {
            st->codecpar->codec_type = AVMEDIA_TYPE_SUBTITLE;
            if (st->codecpar->codec_id == AV_CODEC_ID_ASS)
                matroska->contains_ssa = 1;
        }
    }

    return 0;
}
