static void opt_output_file(const char *filename)
{
    AVFormatContext *oc;
    int err, use_video, use_audio, use_subtitle;
    int input_has_video, input_has_audio, input_has_subtitle;
    AVFormatParameters params, *ap = &params;
    AVOutputFormat *file_oformat;

    if (!strcmp(filename, "-"))
        filename = "pipe:";

    oc = avformat_alloc_context();
    if (!oc) {
        print_error(filename, AVERROR(ENOMEM));
        ffmpeg_exit(1);
    }

    if (last_asked_format) {
        file_oformat = av_guess_format(last_asked_format, NULL, NULL);
        if (!file_oformat) {
            fprintf(stderr, "Requested output format '%s' is not a suitable output format\n", last_asked_format);
            ffmpeg_exit(1);
        }
        last_asked_format = NULL;
    } else {
        file_oformat = av_guess_format(NULL, filename, NULL);
        if (!file_oformat) {
            fprintf(stderr, "Unable to find a suitable output format for '%s'\n",
                    filename);
            ffmpeg_exit(1);
        }
    }

    oc->oformat = file_oformat;
    av_strlcpy(oc->filename, filename, sizeof(oc->filename));

    if (!strcmp(file_oformat->name, "ffm") &&
        av_strstart(filename, "http:", NULL)) {
        /
        int err = read_ffserver_streams(oc, filename);
        if (err < 0) {
            print_error(filename, err);
            ffmpeg_exit(1);
        }
    } else {
        use_video = file_oformat->video_codec != CODEC_ID_NONE || video_stream_copy || video_codec_name;
        use_audio = file_oformat->audio_codec != CODEC_ID_NONE || audio_stream_copy || audio_codec_name;
        use_subtitle = file_oformat->subtitle_codec != CODEC_ID_NONE || subtitle_stream_copy || subtitle_codec_name;

        /
        if (nb_input_files > 0) {
            check_audio_video_sub_inputs(&input_has_video, &input_has_audio,
                                         &input_has_subtitle);
            if (!input_has_video)
                use_video = 0;
            if (!input_has_audio)
                use_audio = 0;
            if (!input_has_subtitle)
                use_subtitle = 0;
        }

        /
        if (audio_disable) {
            use_audio = 0;
        }
        if (video_disable) {
            use_video = 0;
        }
        if (subtitle_disable) {
            use_subtitle = 0;
        }

        if (use_video) {
            new_video_stream(oc);
        }

        if (use_audio) {
            new_audio_stream(oc);
        }

        if (use_subtitle) {
            new_subtitle_stream(oc);
        }

        oc->timestamp = recording_timestamp;

        for(; metadata_count>0; metadata_count--){
            av_metadata_set2(&oc->metadata, metadata[metadata_count-1].key,
                                            metadata[metadata_count-1].value, 0);
        }
        av_metadata_conv(oc, oc->oformat->metadata_conv, NULL);
    }

    output_files[nb_output_files++] = oc;

    /
    if (oc->oformat->flags & AVFMT_NEEDNUMBER) {
        if (!av_filename_number_test(oc->filename)) {
            print_error(oc->filename, AVERROR_NUMEXPECTED);
            ffmpeg_exit(1);
        }
    }

    if (!(oc->oformat->flags & AVFMT_NOFILE)) {
        /
        if (!file_overwrite &&
            (strchr(filename, ':') == NULL ||
             filename[1] == ':' ||
             av_strstart(filename, "file:", NULL))) {
            if (url_exist(filename)) {
                if (!using_stdin) {
                    fprintf(stderr,"File '%s' already exists. Overwrite ? [y/N] ", filename);
                    fflush(stderr);
                    if (!read_yesno()) {
                        fprintf(stderr, "Not overwriting - exiting\n");
                        ffmpeg_exit(1);
                    }
                }
                else {
                    fprintf(stderr,"File '%s' already exists. Exiting.\n", filename);
                    ffmpeg_exit(1);
                }
            }
        }

        /
        if ((err = url_fopen(&oc->pb, filename, URL_WRONLY)) < 0) {
            print_error(filename, err);
            ffmpeg_exit(1);
        }
    }

    memset(ap, 0, sizeof(*ap));
    if (av_set_parameters(oc, ap) < 0) {
        fprintf(stderr, "%s: Invalid encoding parameters\n",
                oc->filename);
        ffmpeg_exit(1);
    }

    oc->preload= (int)(mux_preload*AV_TIME_BASE);
    oc->max_delay= (int)(mux_max_delay*AV_TIME_BASE);
    oc->loop_output = loop_output;
    oc->flags |= AVFMT_FLAG_NONBLOCK;

    set_context_opts(oc, avformat_opts, AV_OPT_FLAG_ENCODING_PARAM, NULL);

    nb_streamid_map = 0;
}
