int avformat_find_stream_info(AVFormatContext *ic, AVDictionary **options)
{
    int i, count, ret, read_size, j;
    AVStream *st;
    AVPacket pkt1, *pkt;
    int64_t old_offset  = avio_tell(ic->pb);
    // new streams might appear, no options for those
    int orig_nb_streams = ic->nb_streams;

    for (i = 0; i < ic->nb_streams; i++) {
        const AVCodec *codec;
        AVDictionary *thread_opt = NULL;
        st = ic->streams[i];

        // only for the split stuff
        if (!st->parser && !(ic->flags & AVFMT_FLAG_NOPARSE)) {
            st->parser = av_parser_init(st->codec->codec_id);
            if (st->need_parsing == AVSTREAM_PARSE_HEADERS && st->parser)
                st->parser->flags |= PARSER_FLAG_COMPLETE_FRAMES;
        }
        codec = st->codec->codec ? st->codec->codec
                                 : avcodec_find_decoder(st->codec->codec_id);

        /
        av_dict_set(options ? &options[i] : &thread_opt, "threads", "1", 0);

        /
        if (st->codec->codec_type == AVMEDIA_TYPE_SUBTITLE
            && codec && !st->codec->codec)
            avcodec_open2(st->codec, codec,
                          options ? &options[i] : &thread_opt);

        // Try to just open decoders, in case this is enough to get parameters.
        if (!has_codec_parameters(st)) {
            if (codec && !st->codec->codec)
                avcodec_open2(st->codec, codec,
                              options ? &options[i] : &thread_opt);
        }
        if (!options)
            av_dict_free(&thread_opt);
    }

    for (i = 0; i < ic->nb_streams; i++) {
        ic->streams[i]->info->fps_first_dts = AV_NOPTS_VALUE;
        ic->streams[i]->info->fps_last_dts  = AV_NOPTS_VALUE;
    }

    count     = 0;
    read_size = 0;
    for (;;) {
        if (ff_check_interrupt(&ic->interrupt_callback)) {
            ret = AVERROR_EXIT;
            av_log(ic, AV_LOG_DEBUG, "interrupted\n");
            break;
        }

        /
        for (i = 0; i < ic->nb_streams; i++) {
            int fps_analyze_framecount = 20;

            st = ic->streams[i];
            if (!has_codec_parameters(st))
                break;
            /
            if (av_q2d(st->time_base) > 0.0005)
                fps_analyze_framecount *= 2;
            if (ic->fps_probe_size >= 0)
                fps_analyze_framecount = ic->fps_probe_size;
            /
            if (tb_unreliable(st->codec) && !st->avg_frame_rate.num &&
                st->codec_info_nb_frames < fps_analyze_framecount &&
                st->codec->codec_type == AVMEDIA_TYPE_VIDEO)
                break;
            if (st->parser && st->parser->parser->split &&
                !st->codec->extradata)
                break;
            if (st->first_dts == AV_NOPTS_VALUE &&
                (st->codec->codec_type == AVMEDIA_TYPE_VIDEO ||
                 st->codec->codec_type == AVMEDIA_TYPE_AUDIO))
                break;
        }
        if (i == ic->nb_streams) {
            /
            if (!(ic->ctx_flags & AVFMTCTX_NOHEADER)) {
                /
                ret = count;
                av_log(ic, AV_LOG_DEBUG, "All info found\n");
                break;
            }
        }
        /
        if (read_size >= ic->probesize) {
            ret = count;
            av_log(ic, AV_LOG_DEBUG,
                   "Probe buffer size limit %d reached\n", ic->probesize);
            break;
        }

        /
        ret = read_frame_internal(ic, &pkt1);
        if (ret == AVERROR(EAGAIN))
            continue;

        if (ret < 0) {
            /
            AVPacket empty_pkt = { 0 };
            int err = 0;
            av_init_packet(&empty_pkt);

            /
            ret = -1;
            for (i = 0; i < ic->nb_streams; i++) {
                st = ic->streams[i];

                /
                if (st->info->found_decoder == 1) {
                    do {
                        err = try_decode_frame(st, &empty_pkt,
                                               (options && i < orig_nb_streams)
                                               ? &options[i] : NULL);
                    } while (err > 0 && !has_codec_parameters(st));
                }

                if (err < 0) {
                    av_log(ic, AV_LOG_WARNING,
                           "decoding for stream %d failed\n", st->index);
                } else if (!has_codec_parameters(st)) {
                    char buf[256];
                    avcodec_string(buf, sizeof(buf), st->codec, 0);
                    av_log(ic, AV_LOG_WARNING,
                           "Could not find codec parameters (%s)\n", buf);
                } else {
                    ret = 0;
                }
            }
            break;
        }

        if (ic->flags & AVFMT_FLAG_NOBUFFER) {
            pkt = &pkt1;
        } else {
            pkt = add_to_pktbuf(&ic->packet_buffer, &pkt1,
                                &ic->packet_buffer_end);
            if ((ret = av_dup_packet(pkt)) < 0)
                goto find_stream_info_err;
        }

        read_size += pkt->size;

        st = ic->streams[pkt->stream_index];
        if (pkt->dts != AV_NOPTS_VALUE && st->codec_info_nb_frames > 1) {
            /
            if (st->info->fps_last_dts != AV_NOPTS_VALUE &&
                st->info->fps_last_dts >= pkt->dts) {
                av_log(ic, AV_LOG_WARNING,
                       "Non-increasing DTS in stream %d: packet %d with DTS "
                       "%"PRId64", packet %d with DTS %"PRId64"\n",
                       st->index, st->info->fps_last_dts_idx,
                       st->info->fps_last_dts, st->codec_info_nb_frames,
                       pkt->dts);
                st->info->fps_first_dts =
                st->info->fps_last_dts  = AV_NOPTS_VALUE;
            }
            /
            if (st->info->fps_last_dts != AV_NOPTS_VALUE &&
                st->info->fps_last_dts_idx > st->info->fps_first_dts_idx &&
                (pkt->dts - st->info->fps_last_dts) / 1000 >
                (st->info->fps_last_dts     - st->info->fps_first_dts) /
                (st->info->fps_last_dts_idx - st->info->fps_first_dts_idx)) {
                av_log(ic, AV_LOG_WARNING,
                       "DTS discontinuity in stream %d: packet %d with DTS "
                       "%"PRId64", packet %d with DTS %"PRId64"\n",
                       st->index, st->info->fps_last_dts_idx,
                       st->info->fps_last_dts, st->codec_info_nb_frames,
                       pkt->dts);
                st->info->fps_first_dts =
                st->info->fps_last_dts  = AV_NOPTS_VALUE;
            }

            /
            if (st->info->fps_first_dts == AV_NOPTS_VALUE) {
                st->info->fps_first_dts     = pkt->dts;
                st->info->fps_first_dts_idx = st->codec_info_nb_frames;
            }
            st->info->fps_last_dts     = pkt->dts;
            st->info->fps_last_dts_idx = st->codec_info_nb_frames;

            /
            if (av_rescale_q(pkt->dts - st->info->fps_first_dts, st->time_base,
                             AV_TIME_BASE_Q) >= ic->max_analyze_duration) {
                av_log(ic, AV_LOG_WARNING, "max_analyze_duration reached\n");
                break;
            }
        }
        if (st->parser && st->parser->parser->split && !st->codec->extradata) {
            int i = st->parser->parser->split(st->codec, pkt->data, pkt->size);
            if (i > 0 && i < FF_MAX_EXTRADATA_SIZE) {
                st->codec->extradata_size = i;
                st->codec->extradata = av_malloc(st->codec->extradata_size +
                                                 FF_INPUT_BUFFER_PADDING_SIZE);
                if (!st->codec->extradata)
                    return AVERROR(ENOMEM);
                memcpy(st->codec->extradata, pkt->data,
                       st->codec->extradata_size);
                memset(st->codec->extradata + i, 0,
                       FF_INPUT_BUFFER_PADDING_SIZE);
            }
        }

        /
        try_decode_frame(st, pkt,
                         (options && i < orig_nb_streams) ? &options[i] : NULL);

        st->codec_info_nb_frames++;
        count++;
    }

    // close codecs which were opened in try_decode_frame()
    for (i = 0; i < ic->nb_streams; i++) {
        st = ic->streams[i];
        avcodec_close(st->codec);
    }
    for (i = 0; i < ic->nb_streams; i++) {
        st = ic->streams[i];
        if (st->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            /
            if (!st->avg_frame_rate.num &&
                st->info->fps_last_dts != st->info->fps_first_dts) {
                int64_t delta_dts = st->info->fps_last_dts -
                                    st->info->fps_first_dts;
                int delta_packets = st->info->fps_last_dts_idx -
                                    st->info->fps_first_dts_idx;
                int best_fps      = 0;
                double best_error = 0.01;

                if (delta_dts     >= INT64_MAX / st->time_base.num ||
                    delta_packets >= INT64_MAX / st->time_base.den ||
                    delta_dts < 0)
                    continue;
                av_reduce(&st->avg_frame_rate.num, &st->avg_frame_rate.den,
                          delta_packets * (int64_t) st->time_base.den,
                          delta_dts * (int64_t) st->time_base.num, 60000);

                /
                for (j = 1; j < MAX_STD_TIMEBASES; j++) {
                    AVRational std_fps = { get_std_framerate(j), 12 * 1001 };
                    double error       = fabs(av_q2d(st->avg_frame_rate) /
                                              av_q2d(std_fps) - 1);

                    if (error < best_error) {
                        best_error = error;
                        best_fps   = std_fps.num;
                    }
                }
                if (best_fps)
                    av_reduce(&st->avg_frame_rate.num, &st->avg_frame_rate.den,
                              best_fps, 12 * 1001, INT_MAX);
            }
        } else if (st->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (!st->codec->bits_per_coded_sample)
                st->codec->bits_per_coded_sample =
                    av_get_bits_per_sample(st->codec->codec_id);
            // set stream disposition based on audio service type
            switch (st->codec->audio_service_type) {
            case AV_AUDIO_SERVICE_TYPE_EFFECTS:
                st->disposition = AV_DISPOSITION_CLEAN_EFFECTS;
                break;
            case AV_AUDIO_SERVICE_TYPE_VISUALLY_IMPAIRED:
                st->disposition = AV_DISPOSITION_VISUAL_IMPAIRED;
                break;
            case AV_AUDIO_SERVICE_TYPE_HEARING_IMPAIRED:
                st->disposition = AV_DISPOSITION_HEARING_IMPAIRED;
                break;
            case AV_AUDIO_SERVICE_TYPE_COMMENTARY:
                st->disposition = AV_DISPOSITION_COMMENT;
                break;
            case AV_AUDIO_SERVICE_TYPE_KARAOKE:
                st->disposition = AV_DISPOSITION_KARAOKE;
                break;
            }
        }
    }

    estimate_timings(ic, old_offset);

    compute_chapters_end(ic);

find_stream_info_err:
    for (i = 0; i < ic->nb_streams; i++) {
        ic->streams[i]->codec->thread_count = 0;
        av_freep(&ic->streams[i]->info);
    }
    return ret;
}
