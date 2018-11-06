int avformat_find_stream_info(AVFormatContext *ic, AVDictionary **options)
{
    int i, count, ret, read_size, j;
    AVStream *st;
    AVPacket pkt1, *pkt;
    int64_t old_offset = avio_tell(ic->pb);
    int orig_nb_streams = ic->nb_streams;        // new streams might appear, no options for those
    int flush_codecs = ic->probesize > 0;

    if(ic->pb)
        av_log(ic, AV_LOG_DEBUG, "File position before avformat_find_stream_info() is %"PRId64"\n", avio_tell(ic->pb));

    for(i=0;i<ic->nb_streams;i++) {
        const AVCodec *codec;
        AVDictionary *thread_opt = NULL;
        st = ic->streams[i];

        if (st->codec->codec_type == AVMEDIA_TYPE_VIDEO ||
            st->codec->codec_type == AVMEDIA_TYPE_SUBTITLE) {
/
            if(!st->codec->time_base.num)
                st->codec->time_base= st->time_base;
        }
        //only for the split stuff
        if (!st->parser && !(ic->flags & AVFMT_FLAG_NOPARSE)) {
            st->parser = av_parser_init(st->codec->codec_id);
            if(st->parser){
                if(st->need_parsing == AVSTREAM_PARSE_HEADERS){
                    st->parser->flags |= PARSER_FLAG_COMPLETE_FRAMES;
                } else if(st->need_parsing == AVSTREAM_PARSE_FULL_RAW) {
                    st->parser->flags |= PARSER_FLAG_USE_CODEC_TS;
                }
            } else if (st->need_parsing) {
                av_log(ic, AV_LOG_VERBOSE, "parser not found for codec "
                       "%s, packets or times may be invalid.\n",
                       avcodec_get_name(st->codec->codec_id));
            }
        }
        codec = st->codec->codec ? st->codec->codec :
                                   avcodec_find_decoder(st->codec->codec_id);

        /
        av_dict_set(options ? &options[i] : &thread_opt, "threads", "1", 0);

        /
        if (st->codec->codec_type == AVMEDIA_TYPE_SUBTITLE
            && codec && !st->codec->codec)
            avcodec_open2(st->codec, codec, options ? &options[i]
                              : &thread_opt);

        //try to just open decoders, in case this is enough to get parameters
        if (!has_codec_parameters(st, NULL) && st->request_probe <= 0) {
            if (codec && !st->codec->codec)
                avcodec_open2(st->codec, codec, options ? &options[i]
                              : &thread_opt);
        }
        if (!options)
            av_dict_free(&thread_opt);
    }

    for (i=0; i<ic->nb_streams; i++) {
#if FF_API_R_FRAME_RATE
        ic->streams[i]->info->last_dts = AV_NOPTS_VALUE;
#endif
        ic->streams[i]->info->fps_first_dts = AV_NOPTS_VALUE;
        ic->streams[i]->info->fps_last_dts  = AV_NOPTS_VALUE;
    }

    count = 0;
    read_size = 0;
    for(;;) {
        if (ff_check_interrupt(&ic->interrupt_callback)){
            ret= AVERROR_EXIT;
            av_log(ic, AV_LOG_DEBUG, "interrupted\n");
            break;
        }

        /
        for(i=0;i<ic->nb_streams;i++) {
            int fps_analyze_framecount = 20;

            st = ic->streams[i];
            if (!has_codec_parameters(st, NULL))
                break;
            /
            if (av_q2d(st->time_base) > 0.0005)
                fps_analyze_framecount *= 2;
            if (ic->fps_probe_size >= 0)
                fps_analyze_framecount = ic->fps_probe_size;
            /
            if(   tb_unreliable(st->codec) && !(st->r_frame_rate.num && st->avg_frame_rate.num)
               && st->info->duration_count < fps_analyze_framecount
               && st->codec->codec_type == AVMEDIA_TYPE_VIDEO)
                break;
            if(st->parser && st->parser->parser->split && !st->codec->extradata)
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
                flush_codecs = 0;
                break;
            }
        }
        /
        if (read_size >= ic->probesize) {
            ret = count;
            av_log(ic, AV_LOG_DEBUG, "Probe buffer size limit %d reached\n", ic->probesize);
            for (i = 0; i < ic->nb_streams; i++)
                if (!ic->streams[i]->r_frame_rate.num &&
                    ic->streams[i]->info->duration_count <= 1)
                    av_log(ic, AV_LOG_WARNING,
                           "Stream #%d: not enough frames to estimate rate; "
                           "consider increasing probesize\n", i);
            break;
        }

        /
        ret = read_frame_internal(ic, &pkt1);
        if (ret == AVERROR(EAGAIN))
            continue;

        if (ret < 0) {
            /
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
                av_log(ic, AV_LOG_DEBUG, "Non-increasing DTS in stream %d: "
                       "packet %d with DTS %"PRId64", packet %d with DTS "
                       "%"PRId64"\n", st->index, st->info->fps_last_dts_idx,
                       st->info->fps_last_dts, st->codec_info_nb_frames, pkt->dts);
                st->info->fps_first_dts = st->info->fps_last_dts = AV_NOPTS_VALUE;
            }
            /
            if (st->info->fps_last_dts != AV_NOPTS_VALUE &&
                st->info->fps_last_dts_idx > st->info->fps_first_dts_idx &&
                (pkt->dts - st->info->fps_last_dts) / 1000 >
                (st->info->fps_last_dts - st->info->fps_first_dts) / (st->info->fps_last_dts_idx - st->info->fps_first_dts_idx)) {
                av_log(ic, AV_LOG_WARNING, "DTS discontinuity in stream %d: "
                       "packet %d with DTS %"PRId64", packet %d with DTS "
                       "%"PRId64"\n", st->index, st->info->fps_last_dts_idx,
                       st->info->fps_last_dts, st->codec_info_nb_frames, pkt->dts);
                st->info->fps_first_dts = st->info->fps_last_dts = AV_NOPTS_VALUE;
            }

            /
            if (st->info->fps_first_dts == AV_NOPTS_VALUE) {
                st->info->fps_first_dts     = pkt->dts;
                st->info->fps_first_dts_idx = st->codec_info_nb_frames;
            }
            st->info->fps_last_dts = pkt->dts;
            st->info->fps_last_dts_idx = st->codec_info_nb_frames;
        }
        if (st->codec_info_nb_frames>1) {
            int64_t t=0;
            if (st->time_base.den > 0)
                t = av_rescale_q(st->info->codec_info_duration, st->time_base, AV_TIME_BASE_Q);
            if (st->avg_frame_rate.num > 0)
                t = FFMAX(t, av_rescale_q(st->codec_info_nb_frames, av_inv_q(st->avg_frame_rate), AV_TIME_BASE_Q));

            if (t >= ic->max_analyze_duration) {
                av_log(ic, AV_LOG_WARNING, "max_analyze_duration %d reached at %"PRId64"\n", ic->max_analyze_duration, t);
                break;
            }
            if (pkt->duration) {
                st->info->codec_info_duration        += pkt->duration;
                st->info->codec_info_duration_fields += st->parser && st->codec->ticks_per_frame==2 ? st->parser->repeat_pict + 1 : 2;
            }
        }
#if FF_API_R_FRAME_RATE
        {
            int64_t last = st->info->last_dts;

            if(pkt->dts != AV_NOPTS_VALUE && last != AV_NOPTS_VALUE && pkt->dts > last){
                double dts= (is_relative(pkt->dts) ?  pkt->dts - RELATIVE_TS_BASE : pkt->dts) * av_q2d(st->time_base);
                int64_t duration= pkt->dts - last;

//                 if(st->codec->codec_type == AVMEDIA_TYPE_VIDEO)
//                     av_log(NULL, AV_LOG_ERROR, "%f\n", dts);
                for (i=0; i<FF_ARRAY_ELEMS(st->info->duration_error[0][0]); i++) {
                    int framerate= get_std_framerate(i);
                    double sdts= dts*framerate/(1001*12);
                    for(j=0; j<2; j++){
                        int ticks= lrintf(sdts+j*0.5);
                        double error= sdts - ticks + j*0.5;
                        st->info->duration_error[j][0][i] += error;
                        st->info->duration_error[j][1][i] += error*error;
                    }
                }
                st->info->duration_count++;
                // ignore the first 4 values, they might have some random jitter
                if (st->info->duration_count > 3)
                    st->info->duration_gcd = av_gcd(st->info->duration_gcd, duration);
            }
            if (pkt->dts != AV_NOPTS_VALUE)
                st->info->last_dts = pkt->dts;
        }
#endif
        if(st->parser && st->parser->parser->split && !st->codec->extradata){
            int i= st->parser->parser->split(st->codec, pkt->data, pkt->size);
            if (i > 0 && i < FF_MAX_EXTRADATA_SIZE) {
                st->codec->extradata_size= i;
                st->codec->extradata= av_malloc(st->codec->extradata_size + FF_INPUT_BUFFER_PADDING_SIZE);
                if (!st->codec->extradata)
                    return AVERROR(ENOMEM);
                memcpy(st->codec->extradata, pkt->data, st->codec->extradata_size);
                memset(st->codec->extradata + i, 0, FF_INPUT_BUFFER_PADDING_SIZE);
            }
        }

        /
        try_decode_frame(st, pkt, (options && i < orig_nb_streams ) ? &options[i] : NULL);

        st->codec_info_nb_frames++;
        count++;
    }

    if (flush_codecs) {
        AVPacket empty_pkt = { 0 };
        int err = 0;
        av_init_packet(&empty_pkt);

        ret = -1; /
        for(i=0;i<ic->nb_streams;i++) {
            const char *errmsg;

            st = ic->streams[i];

            /
            if (st->info->found_decoder == 1) {
                do {
                    err = try_decode_frame(st, &empty_pkt,
                                            (options && i < orig_nb_streams) ?
                                            &options[i] : NULL);
                } while (err > 0 && !has_codec_parameters(st, NULL));

                if (err < 0) {
                    av_log(ic, AV_LOG_INFO,
                        "decoding for stream %d failed\n", st->index);
                }
            }

            if (!has_codec_parameters(st, &errmsg)) {
                char buf[256];
                avcodec_string(buf, sizeof(buf), st->codec, 0);
                av_log(ic, AV_LOG_WARNING,
                       "Could not find codec parameters for stream %d (%s): %s\n"
                       "Consider increasing the value for the 'analyzeduration' and 'probesize' options\n",
                       i, buf, errmsg);
            } else {
                ret = 0;
            }
        }
    }

    // close codecs which were opened in try_decode_frame()
    for(i=0;i<ic->nb_streams;i++) {
        st = ic->streams[i];
        avcodec_close(st->codec);
    }
    for(i=0;i<ic->nb_streams;i++) {
        st = ic->streams[i];
        if (st->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            if(st->codec->codec_id == AV_CODEC_ID_RAWVIDEO && !st->codec->codec_tag && !st->codec->bits_per_coded_sample){
                uint32_t tag= avcodec_pix_fmt_to_codec_tag(st->codec->pix_fmt);
                if(ff_find_pix_fmt(ff_raw_pix_fmt_tags, tag) == st->codec->pix_fmt)
                    st->codec->codec_tag= tag;
            }

            /
            if (st->info->codec_info_duration_fields && !st->avg_frame_rate.num && st->info->codec_info_duration) {
                int      best_fps = 0;
                double best_error = 0.01;

                av_reduce(&st->avg_frame_rate.num, &st->avg_frame_rate.den,
                          st->info->codec_info_duration_fields*(int64_t)st->time_base.den,
                          st->info->codec_info_duration*2*(int64_t)st->time_base.num, 60000);

                /
                for (j = 1; j < MAX_STD_TIMEBASES; j++) {
                    AVRational std_fps = { get_std_framerate(j), 12*1001 };
                    double error = fabs(av_q2d(st->avg_frame_rate) / av_q2d(std_fps) - 1);

                    if (error < best_error) {
                        best_error = error;
                        best_fps   = std_fps.num;
                    }
                }
                if (best_fps) {
                    av_reduce(&st->avg_frame_rate.num, &st->avg_frame_rate.den,
                              best_fps, 12*1001, INT_MAX);
                }
            }
            // the check for tb_unreliable() is not completely correct, since this is not about handling
            // a unreliable/inexact time base, but a time base that is finer than necessary, as e.g.
            // ipmovie.c produces.
            if (tb_unreliable(st->codec) && st->info->duration_count > 15 && st->info->duration_gcd > FFMAX(1, st->time_base.den/(500LL*st->time_base.num)) && !st->r_frame_rate.num)
                av_reduce(&st->r_frame_rate.num, &st->r_frame_rate.den, st->time_base.den, st->time_base.num * st->info->duration_gcd, INT_MAX);
            if (st->info->duration_count && !st->r_frame_rate.num
                && tb_unreliable(st->codec)) {
                int num = 0;
                double best_error= 0.01;

                for (j=0; j<FF_ARRAY_ELEMS(st->info->duration_error[0][0]); j++) {
                    int k;

                    if(st->info->codec_info_duration && st->info->codec_info_duration*av_q2d(st->time_base) < (1001*12.0)/get_std_framerate(j))
                        continue;
                    if(!st->info->codec_info_duration && 1.0 < (1001*12.0)/get_std_framerate(j))
                        continue;
                    for(k=0; k<2; k++){
                        int n= st->info->duration_count;
                        double a= st->info->duration_error[k][0][j] / n;
                        double error= st->info->duration_error[k][1][j]/n - a*a;

                        if(error < best_error && best_error> 0.000000001){
                            best_error= error;
                            num = get_std_framerate(j);
                        }
                        if(error < 0.02)
                            av_log(NULL, AV_LOG_DEBUG, "rfps: %f %f\n", get_std_framerate(j) / 12.0/1001, error);
                    }
                }
                // do not increase frame rate by more than 1 % in order to match a standard rate.
                if (num && (!st->r_frame_rate.num || (double)num/(12*1001) < 1.01 * av_q2d(st->r_frame_rate)))
                    av_reduce(&st->r_frame_rate.num, &st->r_frame_rate.den, num, 12*1001, INT_MAX);
            }

            if (!st->r_frame_rate.num){
                if(    st->codec->time_base.den * (int64_t)st->time_base.num
                    <= st->codec->time_base.num * st->codec->ticks_per_frame * (int64_t)st->time_base.den){
                    st->r_frame_rate.num = st->codec->time_base.den;
                    st->r_frame_rate.den = st->codec->time_base.num * st->codec->ticks_per_frame;
                }else{
                    st->r_frame_rate.num = st->time_base.den;
                    st->r_frame_rate.den = st->time_base.num;
                }
            }
        }else if(st->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            if(!st->codec->bits_per_coded_sample)
                st->codec->bits_per_coded_sample= av_get_bits_per_sample(st->codec->codec_id);
            // set stream disposition based on audio service type
            switch (st->codec->audio_service_type) {
            case AV_AUDIO_SERVICE_TYPE_EFFECTS:
                st->disposition = AV_DISPOSITION_CLEAN_EFFECTS;    break;
            case AV_AUDIO_SERVICE_TYPE_VISUALLY_IMPAIRED:
                st->disposition = AV_DISPOSITION_VISUAL_IMPAIRED;  break;
            case AV_AUDIO_SERVICE_TYPE_HEARING_IMPAIRED:
                st->disposition = AV_DISPOSITION_HEARING_IMPAIRED; break;
            case AV_AUDIO_SERVICE_TYPE_COMMENTARY:
                st->disposition = AV_DISPOSITION_COMMENT;          break;
            case AV_AUDIO_SERVICE_TYPE_KARAOKE:
                st->disposition = AV_DISPOSITION_KARAOKE;          break;
            }
        }
    }

    if(ic->probesize)
    estimate_timings(ic, old_offset);

    compute_chapters_end(ic);

 find_stream_info_err:
    for (i=0; i < ic->nb_streams; i++) {
        if (ic->streams[i]->codec)
            ic->streams[i]->codec->thread_count = 0;
        av_freep(&ic->streams[i]->info);
    }
    if(ic->pb)
        av_log(ic, AV_LOG_DEBUG, "File position after avformat_find_stream_info() is %"PRId64"\n", avio_tell(ic->pb));
    return ret;
}
