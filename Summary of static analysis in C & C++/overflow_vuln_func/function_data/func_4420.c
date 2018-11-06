static int av_encode(AVFormatContext **output_files,
                     int nb_output_files,
                     AVFormatContext **input_files,
                     int nb_input_files,
                     AVStreamMap *stream_maps, int nb_stream_maps)
{
    int ret = 0, i, j, k, n, nb_istreams = 0, nb_ostreams = 0;
    AVFormatContext *is, *os;
    AVCodecContext *codec, *icodec;
    AVOutputStream *ost, **ost_table = NULL;
    AVInputStream *ist, **ist_table = NULL;
    AVInputFile *file_table;
    char error[1024];
    int key;
    int want_sdp = 1;
    uint8_t no_packet[MAX_FILES]={0};
    int no_packet_count=0;

    file_table= av_mallocz(nb_input_files * sizeof(AVInputFile));
    if (!file_table)
        goto fail;

    /
    j = 0;
    for(i=0;i<nb_input_files;i++) {
        is = input_files[i];
        file_table[i].ist_index = j;
        file_table[i].nb_streams = is->nb_streams;
        j += is->nb_streams;
    }
    nb_istreams = j;

    ist_table = av_mallocz(nb_istreams * sizeof(AVInputStream *));
    if (!ist_table)
        goto fail;

    for(i=0;i<nb_istreams;i++) {
        ist = av_mallocz(sizeof(AVInputStream));
        if (!ist)
            goto fail;
        ist_table[i] = ist;
    }
    j = 0;
    for(i=0;i<nb_input_files;i++) {
        is = input_files[i];
        for(k=0;k<is->nb_streams;k++) {
            ist = ist_table[j++];
            ist->st = is->streams[k];
            ist->file_index = i;
            ist->index = k;
            ist->discard = 1; /

            if (rate_emu) {
                ist->start = av_gettime();
            }
        }
    }

    /
    nb_ostreams = 0;
    for(i=0;i<nb_output_files;i++) {
        os = output_files[i];
        if (!os->nb_streams) {
            dump_format(output_files[i], i, output_files[i]->filename, 1);
            fprintf(stderr, "Output file #%d does not contain any stream\n", i);
            av_exit(1);
        }
        nb_ostreams += os->nb_streams;
    }
    if (nb_stream_maps > 0 && nb_stream_maps != nb_ostreams) {
        fprintf(stderr, "Number of stream maps must match number of output streams\n");
        av_exit(1);
    }

    /
    for(i=0;i<nb_stream_maps;i++) {
        int fi = stream_maps[i].file_index;
        int si = stream_maps[i].stream_index;

        if (fi < 0 || fi > nb_input_files - 1 ||
            si < 0 || si > file_table[fi].nb_streams - 1) {
            fprintf(stderr,"Could not find input stream #%d.%d\n", fi, si);
            av_exit(1);
        }
        fi = stream_maps[i].sync_file_index;
        si = stream_maps[i].sync_stream_index;
        if (fi < 0 || fi > nb_input_files - 1 ||
            si < 0 || si > file_table[fi].nb_streams - 1) {
            fprintf(stderr,"Could not find sync stream #%d.%d\n", fi, si);
            av_exit(1);
        }
    }

    ost_table = av_mallocz(sizeof(AVOutputStream *) * nb_ostreams);
    if (!ost_table)
        goto fail;
    for(i=0;i<nb_ostreams;i++) {
        ost = av_mallocz(sizeof(AVOutputStream));
        if (!ost)
            goto fail;
        ost_table[i] = ost;
    }

    n = 0;
    for(k=0;k<nb_output_files;k++) {
        os = output_files[k];
        for(i=0;i<os->nb_streams;i++,n++) {
            int found;
            ost = ost_table[n];
            ost->file_index = k;
            ost->index = i;
            ost->st = os->streams[i];
            if (nb_stream_maps > 0) {
                ost->source_index = file_table[stream_maps[n].file_index].ist_index +
                    stream_maps[n].stream_index;

                /
                if (ist_table[ost->source_index]->st->codec->codec_type != ost->st->codec->codec_type) {
                    int i= ost->file_index;
                    dump_format(output_files[i], i, output_files[i]->filename, 1);
                    fprintf(stderr, "Codec type mismatch for mapping #%d.%d -> #%d.%d\n",
                        stream_maps[n].file_index, stream_maps[n].stream_index,
                        ost->file_index, ost->index);
                    av_exit(1);
                }

            } else {
                int best_nb_frames=-1;
                    /
                    found = 0;
                    for(j=0;j<nb_istreams;j++) {
                        int skip=0;
                        ist = ist_table[j];
                        if(opt_programid){
                            int pi,si;
                            AVFormatContext *f= input_files[ ist->file_index ];
                            skip=1;
                            for(pi=0; pi<f->nb_programs; pi++){
                                AVProgram *p= f->programs[pi];
                                if(p->id == opt_programid)
                                    for(si=0; si<p->nb_stream_indexes; si++){
                                        if(f->streams[ p->stream_index[si] ] == ist->st)
                                            skip=0;
                                    }
                            }
                        }
                        if (ist->discard && ist->st->discard != AVDISCARD_ALL && !skip &&
                            ist->st->codec->codec_type == ost->st->codec->codec_type) {
                            if(best_nb_frames < ist->st->codec_info_nb_frames){
                                best_nb_frames= ist->st->codec_info_nb_frames;
                                ost->source_index = j;
                                found = 1;
                            }
                        }
                    }

                if (!found) {
                    if(! opt_programid) {
                        /
                        for(j=0;j<nb_istreams;j++) {
                            ist = ist_table[j];
                            if (   ist->st->codec->codec_type == ost->st->codec->codec_type
                                && ist->st->discard != AVDISCARD_ALL) {
                                ost->source_index = j;
                                found = 1;
                            }
                        }
                    }
                    if (!found) {
                        int i= ost->file_index;
                        dump_format(output_files[i], i, output_files[i]->filename, 1);
                        fprintf(stderr, "Could not find input stream matching output stream #%d.%d\n",
                                ost->file_index, ost->index);
                        av_exit(1);
                    }
                }
            }
            ist = ist_table[ost->source_index];
            ist->discard = 0;
            ost->sync_ist = (nb_stream_maps > 0) ?
                ist_table[file_table[stream_maps[n].sync_file_index].ist_index +
                         stream_maps[n].sync_stream_index] : ist;
        }
    }

    /
    for(i=0;i<nb_ostreams;i++) {
        AVMetadataTag *lang;
        ost = ost_table[i];
        os = output_files[ost->file_index];
        ist = ist_table[ost->source_index];

        codec = ost->st->codec;
        icodec = ist->st->codec;

        if ((lang=av_metadata_get(ist->st->metadata, "language", NULL, 0))
            &&   !av_metadata_get(ost->st->metadata, "language", NULL, 0))
            av_metadata_set(&ost->st->metadata, "language", lang->value);

        ost->st->disposition = ist->st->disposition;
        codec->bits_per_raw_sample= icodec->bits_per_raw_sample;
        codec->chroma_sample_location = icodec->chroma_sample_location;

        if (ost->st->stream_copy) {
            /
            codec->codec_id = icodec->codec_id;
            codec->codec_type = icodec->codec_type;

            if(!codec->codec_tag){
                if(   !os->oformat->codec_tag
                   || av_codec_get_id (os->oformat->codec_tag, icodec->codec_tag) == codec->codec_id
                   || av_codec_get_tag(os->oformat->codec_tag, icodec->codec_id) <= 0)
                    codec->codec_tag = icodec->codec_tag;
            }

            codec->bit_rate = icodec->bit_rate;
            codec->extradata= icodec->extradata;
            codec->extradata_size= icodec->extradata_size;
            if(av_q2d(icodec->time_base)*icodec->ticks_per_frame > av_q2d(ist->st->time_base) && av_q2d(ist->st->time_base) < 1.0/1000){
                codec->time_base = icodec->time_base;
                codec->time_base.num *= icodec->ticks_per_frame;
            }else
                codec->time_base = ist->st->time_base;
            switch(codec->codec_type) {
            case CODEC_TYPE_AUDIO:
                if(audio_volume != 256) {
                    fprintf(stderr,"-acodec copy and -vol are incompatible (frames are not decoded)\n");
                    av_exit(1);
                }
                codec->channel_layout = icodec->channel_layout;
                codec->sample_rate = icodec->sample_rate;
                codec->channels = icodec->channels;
                codec->frame_size = icodec->frame_size;
                codec->block_align= icodec->block_align;
                if(codec->block_align == 1 && codec->codec_id == CODEC_ID_MP3)
                    codec->block_align= 0;
                if(codec->codec_id == CODEC_ID_AC3)
                    codec->block_align= 0;
                break;
            case CODEC_TYPE_VIDEO:
                codec->pix_fmt = icodec->pix_fmt;
                codec->width = icodec->width;
                codec->height = icodec->height;
                codec->has_b_frames = icodec->has_b_frames;
                break;
            case CODEC_TYPE_SUBTITLE:
                codec->width = icodec->width;
                codec->height = icodec->height;
                break;
            default:
                abort();
            }
        } else {
            switch(codec->codec_type) {
            case CODEC_TYPE_AUDIO:
                ost->fifo= av_fifo_alloc(1024);
                if(!ost->fifo)
                    goto fail;
                ost->reformat_pair = MAKE_SFMT_PAIR(SAMPLE_FMT_NONE,SAMPLE_FMT_NONE);
                ost->audio_resample = codec->sample_rate != icodec->sample_rate || audio_sync_method > 1;
                icodec->request_channels = codec->channels;
                ist->decoding_needed = 1;
                ost->encoding_needed = 1;
                break;
            case CODEC_TYPE_VIDEO:
                if (ost->st->codec->pix_fmt == PIX_FMT_NONE) {
                    fprintf(stderr, "Video pixel format is unknown, stream cannot be decoded\n");
                    av_exit(1);
                }
                ost->video_crop = ((frame_leftBand + frame_rightBand + frame_topBand + frame_bottomBand) != 0);
                ost->video_pad = ((frame_padleft + frame_padright + frame_padtop + frame_padbottom) != 0);
                ost->video_resample = ((codec->width != icodec->width -
                                (frame_leftBand + frame_rightBand) +
                                (frame_padleft + frame_padright)) ||
                        (codec->height != icodec->height -
                                (frame_topBand  + frame_bottomBand) +
                                (frame_padtop + frame_padbottom)) ||
                        (codec->pix_fmt != icodec->pix_fmt));
                if (ost->video_crop) {
                    ost->topBand    = ost->original_topBand    = frame_topBand;
                    ost->bottomBand = ost->original_bottomBand = frame_bottomBand;
                    ost->leftBand   = ost->original_leftBand   = frame_leftBand;
                    ost->rightBand  = ost->original_rightBand  = frame_rightBand;
                }
                if (ost->video_pad) {
                    ost->padtop = frame_padtop;
                    ost->padleft = frame_padleft;
                    ost->padbottom = frame_padbottom;
                    ost->padright = frame_padright;
                    if (!ost->video_resample) {
                        avcodec_get_frame_defaults(&ost->pict_tmp);
                        if(avpicture_alloc((AVPicture*)&ost->pict_tmp, codec->pix_fmt,
                                         codec->width, codec->height))
                            goto fail;
                    }
                }
                if (ost->video_resample) {
                    avcodec_get_frame_defaults(&ost->pict_tmp);
                    if(avpicture_alloc((AVPicture*)&ost->pict_tmp, codec->pix_fmt,
                                         codec->width, codec->height)) {
                        fprintf(stderr, "Cannot allocate temp picture, check pix fmt\n");
                        av_exit(1);
                    }
                    sws_flags = av_get_int(sws_opts, "sws_flags", NULL);
                    ost->img_resample_ctx = sws_getContext(
                            icodec->width - (frame_leftBand + frame_rightBand),
                            icodec->height - (frame_topBand + frame_bottomBand),
                            icodec->pix_fmt,
                            codec->width - (frame_padleft + frame_padright),
                            codec->height - (frame_padtop + frame_padbottom),
                            codec->pix_fmt,
                            sws_flags, NULL, NULL, NULL);
                    if (ost->img_resample_ctx == NULL) {
                        fprintf(stderr, "Cannot get resampling context\n");
                        av_exit(1);
                    }

                    ost->original_height = icodec->height;
                    ost->original_width  = icodec->width;

                    codec->bits_per_raw_sample= 0;
                }
                ost->resample_height = icodec->height - (frame_topBand  + frame_bottomBand);
                ost->resample_width  = icodec->width  - (frame_leftBand + frame_rightBand);
                ost->resample_pix_fmt= icodec->pix_fmt;
                ost->encoding_needed = 1;
                ist->decoding_needed = 1;
                break;
            case CODEC_TYPE_SUBTITLE:
                ost->encoding_needed = 1;
                ist->decoding_needed = 1;
                break;
            default:
                abort();
                break;
            }
            /
            if (ost->encoding_needed &&
                (codec->flags & (CODEC_FLAG_PASS1 | CODEC_FLAG_PASS2))) {
                char logfilename[1024];
                FILE *f;
                int size;
                char *logbuffer;

                snprintf(logfilename, sizeof(logfilename), "%s-%d.log",
                         pass_logfilename_prefix ? pass_logfilename_prefix : DEFAULT_PASS_LOGFILENAME_PREFIX,
                         i);
                if (codec->flags & CODEC_FLAG_PASS1) {
                    f = fopen(logfilename, "w");
                    if (!f) {
                        fprintf(stderr, "Cannot write log file '%s' for pass-1 encoding: %s\n", logfilename, strerror(errno));
                        av_exit(1);
                    }
                    ost->logfile = f;
                } else {
                    /
                    f = fopen(logfilename, "r");
                    if (!f) {
                        fprintf(stderr, "Cannot read log file '%s' for pass-2 encoding: %s\n", logfilename, strerror(errno));
                        av_exit(1);
                    }
                    fseek(f, 0, SEEK_END);
                    size = ftell(f);
                    fseek(f, 0, SEEK_SET);
                    logbuffer = av_malloc(size + 1);
                    if (!logbuffer) {
                        fprintf(stderr, "Could not allocate log buffer\n");
                        av_exit(1);
                    }
                    size = fread(logbuffer, 1, size, f);
                    fclose(f);
                    logbuffer[size] = '\0';
                    codec->stats_in = logbuffer;
                }
            }
        }
        if(codec->codec_type == CODEC_TYPE_VIDEO){
            int size= codec->width * codec->height;
            bit_buffer_size= FFMAX(bit_buffer_size, 6*size + 200);
        }
    }

    if (!bit_buffer)
        bit_buffer = av_malloc(bit_buffer_size);
    if (!bit_buffer) {
        fprintf(stderr, "Cannot allocate %d bytes output buffer\n",
                bit_buffer_size);
        ret = AVERROR(ENOMEM);
        goto fail;
    }

    /
    for(i=0;i<nb_ostreams;i++) {
        ost = ost_table[i];
        if (ost->encoding_needed) {
            AVCodec *codec = output_codecs[i];
            if (!codec)
                codec = avcodec_find_encoder(ost->st->codec->codec_id);
            if (!codec) {
                snprintf(error, sizeof(error), "Encoder (codec id %d) not found for output stream #%d.%d",
                         ost->st->codec->codec_id, ost->file_index, ost->index);
                ret = AVERROR(EINVAL);
                goto dump_format;
            }
            if (avcodec_open(ost->st->codec, codec) < 0) {
                snprintf(error, sizeof(error), "Error while opening encoder for output stream #%d.%d - maybe incorrect parameters such as bit_rate, rate, width or height",
                        ost->file_index, ost->index);
                ret = AVERROR(EINVAL);
                goto dump_format;
            }
            extra_size += ost->st->codec->extradata_size;
        }
    }

    /
    for(i=0;i<nb_istreams;i++) {
        ist = ist_table[i];
        if (ist->decoding_needed) {
            AVCodec *codec = input_codecs[i];
            if (!codec)
                codec = avcodec_find_decoder(ist->st->codec->codec_id);
            if (!codec) {
                snprintf(error, sizeof(error), "Decoder (codec id %d) not found for input stream #%d.%d",
                        ist->st->codec->codec_id, ist->file_index, ist->index);
                ret = AVERROR(EINVAL);
                goto dump_format;
            }
            if (avcodec_open(ist->st->codec, codec) < 0) {
                snprintf(error, sizeof(error), "Error while opening decoder for input stream #%d.%d",
                        ist->file_index, ist->index);
                ret = AVERROR(EINVAL);
                goto dump_format;
            }
            //if (ist->st->codec->codec_type == CODEC_TYPE_VIDEO)
            //    ist->st->codec->flags |= CODEC_FLAG_REPEAT_FIELD;
        }
    }

    /
    for(i=0;i<nb_istreams;i++) {
        AVStream *st;
        ist = ist_table[i];
        st= ist->st;
        ist->pts = st->avg_frame_rate.num ? - st->codec->has_b_frames*AV_TIME_BASE / av_q2d(st->avg_frame_rate) : 0;
        ist->next_pts = AV_NOPTS_VALUE;
        ist->is_start = 1;
    }

    /
    for (i=0;i<nb_meta_data_maps;i++) {
        AVFormatContext *out_file;
        AVFormatContext *in_file;
        AVMetadataTag *mtag;

        int out_file_index = meta_data_maps[i].out_file;
        int in_file_index = meta_data_maps[i].in_file;
        if (out_file_index < 0 || out_file_index >= nb_output_files) {
            snprintf(error, sizeof(error), "Invalid output file index %d map_meta_data(%d,%d)",
                     out_file_index, out_file_index, in_file_index);
            ret = AVERROR(EINVAL);
            goto dump_format;
        }
        if (in_file_index < 0 || in_file_index >= nb_input_files) {
            snprintf(error, sizeof(error), "Invalid input file index %d map_meta_data(%d,%d)",
                     in_file_index, out_file_index, in_file_index);
            ret = AVERROR(EINVAL);
            goto dump_format;
        }

        out_file = output_files[out_file_index];
        in_file = input_files[in_file_index];


        mtag=NULL;
        while((mtag=av_metadata_get(in_file->metadata, "", mtag, AV_METADATA_IGNORE_SUFFIX)))
            av_metadata_set(&out_file->metadata, mtag->key, mtag->value);
        av_metadata_conv(out_file, out_file->oformat->metadata_conv,
                                    in_file->iformat->metadata_conv);
    }

    /
    for(i=0;i<nb_output_files;i++) {
        os = output_files[i];
        if (av_write_header(os) < 0) {
            snprintf(error, sizeof(error), "Could not write header for output file #%d (incorrect codec parameters ?)", i);
            ret = AVERROR(EINVAL);
            goto dump_format;
        }
        if (strcmp(output_files[i]->oformat->name, "rtp")) {
            want_sdp = 0;
        }
    }

 dump_format:
    /
    for(i=0;i<nb_output_files;i++) {
        dump_format(output_files[i], i, output_files[i]->filename, 1);
    }

    /
    if (verbose >= 0) {
        fprintf(stderr, "Stream mapping:\n");
        for(i=0;i<nb_ostreams;i++) {
            ost = ost_table[i];
            fprintf(stderr, "  Stream #%d.%d -> #%d.%d",
                    ist_table[ost->source_index]->file_index,
                    ist_table[ost->source_index]->index,
                    ost->file_index,
                    ost->index);
            if (ost->sync_ist != ist_table[ost->source_index])
                fprintf(stderr, " [sync #%d.%d]",
                        ost->sync_ist->file_index,
                        ost->sync_ist->index);
            fprintf(stderr, "\n");
        }
    }

    if (ret) {
        fprintf(stderr, "%s\n", error);
        goto fail;
    }

    if (want_sdp) {
        print_sdp(output_files, nb_output_files);
    }

    if (!using_stdin && verbose >= 0) {
        fprintf(stderr, "Press [q] to stop encoding\n");
        url_set_interrupt_cb(decode_interrupt_cb);
    }
    term_init();

    timer_start = av_gettime();

    for(; received_sigterm == 0;) {
        int file_index, ist_index;
        AVPacket pkt;
        double ipts_min;
        double opts_min;

    redo:
        ipts_min= 1e100;
        opts_min= 1e100;
        /
        if (!using_stdin) {
            if (q_pressed)
                break;
            /
            key = read_key();
            if (key == 'q')
                break;
        }

        /
        file_index = -1;
        for(i=0;i<nb_ostreams;i++) {
            double ipts, opts;
            ost = ost_table[i];
            os = output_files[ost->file_index];
            ist = ist_table[ost->source_index];
            if(ist->is_past_recording_time || no_packet[ist->file_index])
                continue;
                opts = ost->st->pts.val * av_q2d(ost->st->time_base);
            ipts = (double)ist->pts;
            if (!file_table[ist->file_index].eof_reached){
                if(ipts < ipts_min) {
                    ipts_min = ipts;
                    if(input_sync ) file_index = ist->file_index;
                }
                if(opts < opts_min) {
                    opts_min = opts;
                    if(!input_sync) file_index = ist->file_index;
                }
            }
            if(ost->frame_number >= max_frames[ost->st->codec->codec_type]){
                file_index= -1;
                break;
            }
        }
        /
        if (file_index < 0) {
            if(no_packet_count){
                no_packet_count=0;
                memset(no_packet, 0, sizeof(no_packet));
                usleep(10000);
                continue;
            }
            break;
        }

        /
        if (limit_filesize != 0 && limit_filesize < url_ftell(output_files[0]->pb))
            break;

        /
        is = input_files[file_index];
        ret= av_read_frame(is, &pkt);
        if(ret == AVERROR(EAGAIN)){
            no_packet[file_index]=1;
            no_packet_count++;
            continue;
        }
        if (ret < 0) {
            file_table[file_index].eof_reached = 1;
            if (opt_shortest)
                break;
            else
                continue;
        }

        no_packet_count=0;
        memset(no_packet, 0, sizeof(no_packet));

        if (do_pkt_dump) {
            av_pkt_dump_log(NULL, AV_LOG_DEBUG, &pkt, do_hex_dump);
        }
        /
        if (pkt.stream_index >= file_table[file_index].nb_streams)
            goto discard_packet;
        ist_index = file_table[file_index].ist_index + pkt.stream_index;
        ist = ist_table[ist_index];
        if (ist->discard)
            goto discard_packet;

        if (pkt.dts != AV_NOPTS_VALUE)
            pkt.dts += av_rescale_q(input_files_ts_offset[ist->file_index], AV_TIME_BASE_Q, ist->st->time_base);
        if (pkt.pts != AV_NOPTS_VALUE)
            pkt.pts += av_rescale_q(input_files_ts_offset[ist->file_index], AV_TIME_BASE_Q, ist->st->time_base);

        if(input_files_ts_scale[file_index][pkt.stream_index]){
            if(pkt.pts != AV_NOPTS_VALUE)
                pkt.pts *= input_files_ts_scale[file_index][pkt.stream_index];
            if(pkt.dts != AV_NOPTS_VALUE)
                pkt.dts *= input_files_ts_scale[file_index][pkt.stream_index];
        }

//        fprintf(stderr, "next:%"PRId64" dts:%"PRId64" off:%"PRId64" %d\n", ist->next_pts, pkt.dts, input_files_ts_offset[ist->file_index], ist->st->codec->codec_type);
        if (pkt.dts != AV_NOPTS_VALUE && ist->next_pts != AV_NOPTS_VALUE
            && (is->iformat->flags & AVFMT_TS_DISCONT)) {
            int64_t pkt_dts= av_rescale_q(pkt.dts, ist->st->time_base, AV_TIME_BASE_Q);
            int64_t delta= pkt_dts - ist->next_pts;
            if((FFABS(delta) > 1LL*dts_delta_threshold*AV_TIME_BASE || pkt_dts+1<ist->pts)&& !copy_ts){
                input_files_ts_offset[ist->file_index]-= delta;
                if (verbose > 2)
                    fprintf(stderr, "timestamp discontinuity %"PRId64", new offset= %"PRId64"\n", delta, input_files_ts_offset[ist->file_index]);
                pkt.dts-= av_rescale_q(delta, AV_TIME_BASE_Q, ist->st->time_base);
                if(pkt.pts != AV_NOPTS_VALUE)
                    pkt.pts-= av_rescale_q(delta, AV_TIME_BASE_Q, ist->st->time_base);
            }
        }

        /
        if (av_compare_ts(pkt.pts, ist->st->time_base, recording_time + start_time, (AVRational){1, 1000000}) >= 0) {
            ist->is_past_recording_time = 1;
            goto discard_packet;
        }

        //fprintf(stderr,"read #%d.%d size=%d\n", ist->file_index, ist->index, pkt.size);
        if (output_packet(ist, ist_index, ost_table, nb_ostreams, &pkt) < 0) {

            if (verbose >= 0)
                fprintf(stderr, "Error while decoding stream #%d.%d\n",
                        ist->file_index, ist->index);
            if (exit_on_error)
                av_exit(1);
            av_free_packet(&pkt);
            goto redo;
        }

    discard_packet:
        av_free_packet(&pkt);

        /
        print_report(output_files, ost_table, nb_ostreams, 0);
    }

    /
    for(i=0;i<nb_istreams;i++) {
        ist = ist_table[i];
        if (ist->decoding_needed) {
            output_packet(ist, i, ost_table, nb_ostreams, NULL);
        }
    }

    term_exit();

    /
    for(i=0;i<nb_output_files;i++) {
        os = output_files[i];
        av_write_trailer(os);
    }

    /
    print_report(output_files, ost_table, nb_ostreams, 1);

    /
    for(i=0;i<nb_ostreams;i++) {
        ost = ost_table[i];
        if (ost->encoding_needed) {
            av_freep(&ost->st->codec->stats_in);
            avcodec_close(ost->st->codec);
        }
    }

    /
    for(i=0;i<nb_istreams;i++) {
        ist = ist_table[i];
        if (ist->decoding_needed) {
            avcodec_close(ist->st->codec);
        }
    }

    /
    ret = 0;

 fail:
    av_freep(&bit_buffer);
    av_free(file_table);

    if (ist_table) {
        for(i=0;i<nb_istreams;i++) {
            ist = ist_table[i];
            av_free(ist);
        }
        av_free(ist_table);
    }
    if (ost_table) {
        for(i=0;i<nb_ostreams;i++) {
            ost = ost_table[i];
            if (ost) {
                if (ost->logfile) {
                    fclose(ost->logfile);
                    ost->logfile = NULL;
                }
                av_fifo_free(ost->fifo); /
                av_free(ost->pict_tmp.data[0]);
                if (ost->video_resample)
                    sws_freeContext(ost->img_resample_ctx);
                if (ost->resample)
                    audio_resample_close(ost->resample);
                if (ost->reformat_ctx)
                    av_audio_convert_free(ost->reformat_ctx);
                av_free(ost);
            }
        }
        av_free(ost_table);
    }
    return ret;
}
