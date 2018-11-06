static int mpegts_write_pmt(AVFormatContext *s, MpegTSService *service)
{
    MpegTSWrite *ts = s->priv_data;
    uint8_t data[SECTION_LENGTH], *q, *desc_length_ptr, *program_info_length_ptr;
    int val, stream_type, i, err = 0;

    q = data;
    put16(&q, 0xe000 | service->pcr_pid);

    program_info_length_ptr = q;
    q += 2; /

    /

    val = 0xf000 | (q - program_info_length_ptr - 2);
    program_info_length_ptr[0] = val >> 8;
    program_info_length_ptr[1] = val;

    for (i = 0; i < s->nb_streams; i++) {
        AVStream *st = s->streams[i];
        MpegTSWriteStream *ts_st = st->priv_data;
        AVDictionaryEntry *lang = av_dict_get(st->metadata, "language", NULL, 0);

        if (q - data > SECTION_LENGTH - 32) {
            err = 1;
            break;
        }
        switch (st->codec->codec_id) {
        case AV_CODEC_ID_MPEG1VIDEO:
        case AV_CODEC_ID_MPEG2VIDEO:
            stream_type = STREAM_TYPE_VIDEO_MPEG2;
            break;
        case AV_CODEC_ID_MPEG4:
            stream_type = STREAM_TYPE_VIDEO_MPEG4;
            break;
        case AV_CODEC_ID_H264:
            stream_type = STREAM_TYPE_VIDEO_H264;
            break;
        case AV_CODEC_ID_HEVC:
            stream_type = STREAM_TYPE_VIDEO_HEVC;
            break;
        case AV_CODEC_ID_CAVS:
            stream_type = STREAM_TYPE_VIDEO_CAVS;
            break;
        case AV_CODEC_ID_DIRAC:
            stream_type = STREAM_TYPE_VIDEO_DIRAC;
            break;
        case AV_CODEC_ID_VC1:
            stream_type = STREAM_TYPE_VIDEO_VC1;
            break;
        case AV_CODEC_ID_MP2:
        case AV_CODEC_ID_MP3:
            stream_type = STREAM_TYPE_AUDIO_MPEG1;
            break;
        case AV_CODEC_ID_AAC:
            stream_type = (ts->flags & MPEGTS_FLAG_AAC_LATM)
                          ? STREAM_TYPE_AUDIO_AAC_LATM
                          : STREAM_TYPE_AUDIO_AAC;
            break;
        case AV_CODEC_ID_AAC_LATM:
            stream_type = STREAM_TYPE_AUDIO_AAC_LATM;
            break;
        case AV_CODEC_ID_AC3:
            stream_type = STREAM_TYPE_AUDIO_AC3;
            break;
        case AV_CODEC_ID_DTS:
            stream_type = STREAM_TYPE_AUDIO_DTS;
            break;
        case AV_CODEC_ID_TRUEHD:
            stream_type = STREAM_TYPE_AUDIO_TRUEHD;
            break;
        case AV_CODEC_ID_OPUS:
            stream_type = STREAM_TYPE_PRIVATE_DATA;
            break;
        default:
            stream_type = STREAM_TYPE_PRIVATE_DATA;
            break;
        }

        *q++ = stream_type;
        put16(&q, 0xe000 | ts_st->pid);
        desc_length_ptr = q;
        q += 2; /

        /
        switch (st->codec->codec_type) {
        case AVMEDIA_TYPE_AUDIO:
            if (st->codec->codec_id==AV_CODEC_ID_EAC3) {
                *q++=0x7a; // EAC3 descriptor see A038 DVB SI
                *q++=1; // 1 byte, all flags sets to 0
                *q++=0; // omit all fields...
            }
            if (st->codec->codec_id==AV_CODEC_ID_S302M) {
                *q++ = 0x05; /
                *q++ = 4;
                *q++ = 'B';
                *q++ = 'S';
                *q++ = 'S';
                *q++ = 'D';
            }
            if (st->codec->codec_id==AV_CODEC_ID_OPUS) {
                /
                if (q - data > SECTION_LENGTH - 6 - 4) {
                    err = 1;
                    break;
                }

                *q++ = 0x05; /
                *q++ = 4;
                *q++ = 'O';
                *q++ = 'p';
                *q++ = 'u';
                *q++ = 's';

                *q++ = 0x7f; /
                *q++ = 2;
                *q++ = 0x80;

                if (st->codec->extradata && st->codec->extradata_size >= 19) {
                    if (st->codec->extradata[18] == 0 && st->codec->channels <= 2) {
                        /
                        *q++ = st->codec->channels;
                    } else if (st->codec->extradata[18] == 1 && st->codec->channels <= 8 &&
                               st->codec->extradata_size >= 21 + st->codec->channels) {
                        static const uint8_t coupled_stream_counts[9] = {
                            1, 0, 1, 1, 2, 2, 2, 3, 3
                        };
                        static const uint8_t channel_map_a[8][8] = {
                            {0},
                            {0, 1},
                            {0, 2, 1},
                            {0, 1, 2, 3},
                            {0, 4, 1, 2, 3},
                            {0, 4, 1, 2, 3, 5},
                            {0, 4, 1, 2, 3, 5, 6},
                            {0, 6, 1, 2, 3, 4, 5, 7},
                        };
                        static const uint8_t channel_map_b[8][8] = {
                            {0},
                            {0, 1},
                            {0, 1, 2},
                            {0, 1, 2, 3},
                            {0, 1, 2, 3, 4},
                            {0, 1, 2, 3, 4, 5},
                            {0, 1, 2, 3, 4, 5, 6},
                            {0, 1, 2, 3, 4, 5, 6, 7},
                        };
                        /

                        if (st->codec->extradata[19] == st->codec->channels - coupled_stream_counts[st->codec->channels] &&
                            st->codec->extradata[20] == coupled_stream_counts[st->codec->channels] &&
                            memcmp(&st->codec->extradata[21], channel_map_a[st->codec->channels], st->codec->channels) == 0) {
                            *q++ = st->codec->channels;
                        } else if (st->codec->channels >= 2 && st->codec->extradata[19] == st->codec->channels &&
                                   st->codec->extradata[20] == 0 &&
                                   memcmp(&st->codec->extradata[21], channel_map_b[st->codec->channels], st->codec->channels) == 0) {
                            *q++ = st->codec->channels | 0x80;
                        } else {
                            /
                            av_log(s, AV_LOG_ERROR, "Unsupported Opus Vorbis-style channel mapping");
                            *q++ = 0xff;
                        }
                    } else {
                        /
                        av_log(s, AV_LOG_ERROR, "Unsupported Opus channel mapping for family %d", st->codec->extradata[18]);
                        *q++ = 0xff;
                    }
                } else if (st->codec->channels <= 2) {
                    /
                    *q++ = st->codec->channels;
                } else {
                    /
                    av_log(s, AV_LOG_ERROR, "Unsupported Opus channel mapping");
                    *q++ = 0xff;
                }
            }

            if (lang) {
                char *p;
                char *next = lang->value;
                uint8_t *len_ptr;

                *q++     = 0x0a; /
                len_ptr  = q++;
                *len_ptr = 0;

                for (p = lang->value; next && *len_ptr < 255 / 4 * 4; p = next + 1) {
                    if (q - data > SECTION_LENGTH - 4) {
                        err = 1;
                        break;
                    }
                    next = strchr(p, ',');
                    if (strlen(p) != 3 && (!next || next != p + 3))
                        continue; /

                    *q++ = *p++;
                    *q++ = *p++;
                    *q++ = *p++;

                    if (st->disposition & AV_DISPOSITION_CLEAN_EFFECTS)
                        *q++ = 0x01;
                    else if (st->disposition & AV_DISPOSITION_HEARING_IMPAIRED)
                        *q++ = 0x02;
                    else if (st->disposition & AV_DISPOSITION_VISUAL_IMPAIRED)
                        *q++ = 0x03;
                    else
                        *q++ = 0; /

                    *len_ptr += 4;
                }

                if (*len_ptr == 0)
                    q -= 2; /
            }
            break;
        case AVMEDIA_TYPE_SUBTITLE:
        {
           const char default_language[] = "und";
           const char *language = lang && strlen(lang->value) >= 3 ? lang->value : default_language;

           if (st->codec->codec_id == AV_CODEC_ID_DVB_SUBTITLE) {
               uint8_t *len_ptr;
               int extradata_copied = 0;

               *q++ = 0x59; /
               len_ptr = q++;

               while (strlen(language) >= 3) {
                   if (sizeof(data) - (q - data) < 8) { /
                       err = 1;
                       break;
                   }
                   *q++ = *language++;
                   *q++ = *language++;
                   *q++ = *language++;
                   /
                   if (*language != '\0')
                       language++;

                   if (st->codec->extradata_size - extradata_copied >= 5) {
                       *q++ = st->codec->extradata[extradata_copied + 4]; /
                       memcpy(q, st->codec->extradata + extradata_copied, 4); /
                       extradata_copied += 5;
                       q += 4;
                   } else {
                       /
                       *q++ = (st->disposition & AV_DISPOSITION_HEARING_IMPAIRED) ? 0x20 : 0x10;
                       if ((st->codec->extradata_size == 4) && (extradata_copied == 0)) {
                           /
                           memcpy(q, st->codec->extradata, 4); /
                           extradata_copied += 4;
                           q += 4;
                       } else {
                           put16(&q, 1); /
                           put16(&q, 1); /
                       }
                   }
               }

               *len_ptr = q - len_ptr - 1;
           } else if (st->codec->codec_id == AV_CODEC_ID_DVB_TELETEXT) {
               uint8_t *len_ptr = NULL;
               int extradata_copied = 0;

               /
               *q++ = 0x56;
               len_ptr = q++;

               while (strlen(language) >= 3 && q - data < sizeof(data) - 6) {
                   *q++ = *language++;
                   *q++ = *language++;
                   *q++ = *language++;
                   /
                   if (*language != '\0')
                       language++;

                   if (st->codec->extradata_size - 1 > extradata_copied) {
                       memcpy(q, st->codec->extradata + extradata_copied, 2);
                       extradata_copied += 2;
                       q += 2;
                   } else {
                       /
                       *q++ = 0x08;
                       *q++ = 0x00;
                   }
               }

               *len_ptr = q - len_ptr - 1;
            }
        }
        break;
        case AVMEDIA_TYPE_VIDEO:
            if (stream_type == STREAM_TYPE_VIDEO_DIRAC) {
                *q++ = 0x05; /
                *q++ = 4;
                *q++ = 'd';
                *q++ = 'r';
                *q++ = 'a';
                *q++ = 'c';
            } else if (stream_type == STREAM_TYPE_VIDEO_VC1) {
                *q++ = 0x05; /
                *q++ = 4;
                *q++ = 'V';
                *q++ = 'C';
                *q++ = '-';
                *q++ = '1';
            }
            break;
        case AVMEDIA_TYPE_DATA:
            if (st->codec->codec_id == AV_CODEC_ID_SMPTE_KLV) {
                *q++ = 0x05; /
                *q++ = 4;
                *q++ = 'K';
                *q++ = 'L';
                *q++ = 'V';
                *q++ = 'A';
            }
            break;
        }

        val = 0xf000 | (q - desc_length_ptr - 2);
        desc_length_ptr[0] = val >> 8;
        desc_length_ptr[1] = val;
    }

    if (err)
        av_log(s, AV_LOG_ERROR,
               "The PMT section cannot fit stream %d and all following streams.\n"
               "Try reducing the number of languages in the audio streams "
               "or the total number of streams.\n", i);

    mpegts_write_section1(&service->pmt, PMT_TID, service->sid, ts->tables_version, 0, 0,
                          data, q - data);
    return 0;
}
