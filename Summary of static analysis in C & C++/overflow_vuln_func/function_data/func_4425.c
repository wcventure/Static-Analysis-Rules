static int parse_ffconfig(const char *filename)
{
    FILE *f;
    char line[1024];
    char cmd[64];
    char arg[1024];
    const char *p;
    int val, errors, line_num;
    FFStream **last_stream, *stream, *redirect;
    FFStream **last_feed, *feed;
    AVCodecContext audio_enc, video_enc;
    int audio_id, video_id;

    f = fopen(filename, "r");
    if (!f) {
        perror(filename);
        return -1;
    }
    
    errors = 0;
    line_num = 0;
    first_stream = NULL;
    last_stream = &first_stream;
    first_feed = NULL;
    last_feed = &first_feed;
    stream = NULL;
    feed = NULL;
    redirect = NULL;
    audio_id = CODEC_ID_NONE;
    video_id = CODEC_ID_NONE;
    for(;;) {
        if (fgets(line, sizeof(line), f) == NULL)
            break;
        line_num++;
        p = line;
        while (isspace(*p)) 
            p++;
        if (*p == '\0' || *p == '#')
            continue;

        get_arg(cmd, sizeof(cmd), &p);
        
        if (!strcasecmp(cmd, "Port")) {
            get_arg(arg, sizeof(arg), &p);
            my_http_addr.sin_port = htons (atoi(arg));
        } else if (!strcasecmp(cmd, "BindAddress")) {
            get_arg(arg, sizeof(arg), &p);
            if (!inet_aton(arg, &my_http_addr.sin_addr)) {
                fprintf(stderr, "%s:%d: Invalid IP address: %s\n", 
                        filename, line_num, arg);
                errors++;
            }
        } else if (!strcasecmp(cmd, "NoDaemon")) {
            ffserver_daemon = 0;
        } else if (!strcasecmp(cmd, "RTSPPort")) {
            get_arg(arg, sizeof(arg), &p);
            my_rtsp_addr.sin_port = htons (atoi(arg));
        } else if (!strcasecmp(cmd, "RTSPBindAddress")) {
            get_arg(arg, sizeof(arg), &p);
            if (!inet_aton(arg, &my_rtsp_addr.sin_addr)) {
                fprintf(stderr, "%s:%d: Invalid IP address: %s\n", 
                        filename, line_num, arg);
                errors++;
            }
        } else if (!strcasecmp(cmd, "MaxClients")) {
            get_arg(arg, sizeof(arg), &p);
            val = atoi(arg);
            if (val < 1 || val > HTTP_MAX_CONNECTIONS) {
                fprintf(stderr, "%s:%d: Invalid MaxClients: %s\n", 
                        filename, line_num, arg);
                errors++;
            } else {
                nb_max_connections = val;
            }
        } else if (!strcasecmp(cmd, "MaxBandwidth")) {
            get_arg(arg, sizeof(arg), &p);
            val = atoi(arg);
            if (val < 10 || val > 100000) {
                fprintf(stderr, "%s:%d: Invalid MaxBandwidth: %s\n", 
                        filename, line_num, arg);
                errors++;
            } else {
                max_bandwidth = val;
            }
        } else if (!strcasecmp(cmd, "CustomLog")) {
            get_arg(logfilename, sizeof(logfilename), &p);
        } else if (!strcasecmp(cmd, "<Feed")) {
            /
            /
            char *q;
            if (stream || feed) {
                fprintf(stderr, "%s:%d: Already in a tag\n",
                        filename, line_num);
            } else {
                feed = av_mallocz(sizeof(FFStream));
                /
                *last_stream = feed;
                last_stream = &feed->next;
                /
                *last_feed = feed;
                last_feed = &feed->next_feed;
                
                get_arg(feed->filename, sizeof(feed->filename), &p);
                q = strrchr(feed->filename, '>');
                if (*q)
                    *q = '\0';
                feed->fmt = guess_format("ffm", NULL, NULL);
                /
                snprintf(feed->feed_filename, sizeof(feed->feed_filename),
                         "/tmp/%s.ffm", feed->filename);
                feed->feed_max_size = 5 * 1024 * 1024;
                feed->is_feed = 1;
                feed->feed = feed; /
            }
        } else if (!strcasecmp(cmd, "Launch")) {
            if (feed) {
                int i;

                feed->child_argv = (char **) av_mallocz(64 * sizeof(char *));

                feed->child_argv[0] = av_malloc(7);
                strcpy(feed->child_argv[0], "ffmpeg");

                for (i = 1; i < 62; i++) {
                    char argbuf[256];

                    get_arg(argbuf, sizeof(argbuf), &p);
                    if (!argbuf[0])
                        break;

                    feed->child_argv[i] = av_malloc(strlen(argbuf) + 1);
                    strcpy(feed->child_argv[i], argbuf);
                }

                feed->child_argv[i] = av_malloc(30 + strlen(feed->filename));

                snprintf(feed->child_argv[i], 256, "http://127.0.0.1:%d/%s", 
                    ntohs(my_http_addr.sin_port), feed->filename);
            }
        } else if (!strcasecmp(cmd, "ReadOnlyFile")) {
            if (feed) {
                get_arg(feed->feed_filename, sizeof(feed->feed_filename), &p);
                feed->readonly = 1;
            } else if (stream) {
                get_arg(stream->feed_filename, sizeof(stream->feed_filename), &p);
            }
        } else if (!strcasecmp(cmd, "File")) {
            if (feed) {
                get_arg(feed->feed_filename, sizeof(feed->feed_filename), &p);
            } else if (stream) {
                get_arg(stream->feed_filename, sizeof(stream->feed_filename), &p);
            }
        } else if (!strcasecmp(cmd, "FileMaxSize")) {
            if (feed) {
                const char *p1;
                double fsize;

                get_arg(arg, sizeof(arg), &p);
                p1 = arg;
                fsize = strtod(p1, (char **)&p1);
                switch(toupper(*p1)) {
                case 'K':
                    fsize *= 1024;
                    break;
                case 'M':
                    fsize *= 1024 * 1024;
                    break;
                case 'G':
                    fsize *= 1024 * 1024 * 1024;
                    break;
                }
                feed->feed_max_size = (int64_t)fsize;
            }
        } else if (!strcasecmp(cmd, "</Feed>")) {
            if (!feed) {
                fprintf(stderr, "%s:%d: No corresponding <Feed> for </Feed>\n",
                        filename, line_num);
                errors++;
#if 0
            } else {
                /
                if (unlink(feed->feed_filename) < 0 
                    && errno != ENOENT) {
                    fprintf(stderr, "%s:%d: Unable to clean old feed file '%s': %s\n",
                        filename, line_num, feed->feed_filename, strerror(errno));
                    errors++;
                }
#endif
            }
            feed = NULL;
        } else if (!strcasecmp(cmd, "<Stream")) {
            /
            /
            char *q;
            if (stream || feed) {
                fprintf(stderr, "%s:%d: Already in a tag\n",
                        filename, line_num);
            } else {
                stream = av_mallocz(sizeof(FFStream));
                *last_stream = stream;
                last_stream = &stream->next;

                get_arg(stream->filename, sizeof(stream->filename), &p);
                q = strrchr(stream->filename, '>');
                if (*q)
                    *q = '\0';
                stream->fmt = guess_stream_format(NULL, stream->filename, NULL);
                memset(&audio_enc, 0, sizeof(AVCodecContext));
                memset(&video_enc, 0, sizeof(AVCodecContext));
                audio_id = CODEC_ID_NONE;
                video_id = CODEC_ID_NONE;
                if (stream->fmt) {
                    audio_id = stream->fmt->audio_codec;
                    video_id = stream->fmt->video_codec;
                }
            }
        } else if (!strcasecmp(cmd, "Feed")) {
            get_arg(arg, sizeof(arg), &p);
            if (stream) {
                FFStream *sfeed;
                
                sfeed = first_feed;
                while (sfeed != NULL) {
                    if (!strcmp(sfeed->filename, arg))
                        break;
                    sfeed = sfeed->next_feed;
                }
                if (!sfeed) {
                    fprintf(stderr, "%s:%d: feed '%s' not defined\n",
                            filename, line_num, arg);
                } else {
                    stream->feed = sfeed;
                }
            }
        } else if (!strcasecmp(cmd, "Format")) {
            get_arg(arg, sizeof(arg), &p);
            if (!strcmp(arg, "status")) {
                stream->stream_type = STREAM_TYPE_STATUS;
                stream->fmt = NULL;
            } else {
                stream->stream_type = STREAM_TYPE_LIVE;
                /
                if (!strcmp(arg, "jpeg"))
                    strcpy(arg, "mjpeg");
                stream->fmt = guess_stream_format(arg, NULL, NULL);
                if (!stream->fmt) {
                    fprintf(stderr, "%s:%d: Unknown Format: %s\n", 
                            filename, line_num, arg);
                    errors++;
                }
            }
            if (stream->fmt) {
                audio_id = stream->fmt->audio_codec;
                video_id = stream->fmt->video_codec;
            }
        } else if (!strcasecmp(cmd, "InputFormat")) {
            stream->ifmt = av_find_input_format(arg);
            if (!stream->ifmt) {
                fprintf(stderr, "%s:%d: Unknown input format: %s\n", 
                        filename, line_num, arg);
            }
        } else if (!strcasecmp(cmd, "FaviconURL")) {
            if (stream && stream->stream_type == STREAM_TYPE_STATUS) {
                get_arg(stream->feed_filename, sizeof(stream->feed_filename), &p);
            } else {
                fprintf(stderr, "%s:%d: FaviconURL only permitted for status streams\n", 
                            filename, line_num);
                errors++;
            }
        } else if (!strcasecmp(cmd, "Author")) {
            if (stream) {
                get_arg(stream->author, sizeof(stream->author), &p);
            }
        } else if (!strcasecmp(cmd, "Comment")) {
            if (stream) {
                get_arg(stream->comment, sizeof(stream->comment), &p);
            }
        } else if (!strcasecmp(cmd, "Copyright")) {
            if (stream) {
                get_arg(stream->copyright, sizeof(stream->copyright), &p);
            }
        } else if (!strcasecmp(cmd, "Title")) {
            if (stream) {
                get_arg(stream->title, sizeof(stream->title), &p);
            }
        } else if (!strcasecmp(cmd, "Preroll")) {
            get_arg(arg, sizeof(arg), &p);
            if (stream) {
                stream->prebuffer = atof(arg) * 1000;
            }
        } else if (!strcasecmp(cmd, "StartSendOnKey")) {
            if (stream) {
                stream->send_on_key = 1;
            }
        } else if (!strcasecmp(cmd, "AudioCodec")) {
            get_arg(arg, sizeof(arg), &p);
            audio_id = opt_audio_codec(arg);
            if (audio_id == CODEC_ID_NONE) {
                fprintf(stderr, "%s:%d: Unknown AudioCodec: %s\n", 
                        filename, line_num, arg);
                errors++;
            }
        } else if (!strcasecmp(cmd, "VideoCodec")) {
            get_arg(arg, sizeof(arg), &p);
            video_id = opt_video_codec(arg);
            if (video_id == CODEC_ID_NONE) {
                fprintf(stderr, "%s:%d: Unknown VideoCodec: %s\n", 
                        filename, line_num, arg);
                errors++;
            }
        } else if (!strcasecmp(cmd, "MaxTime")) {
            get_arg(arg, sizeof(arg), &p);
            if (stream) {
                stream->max_time = atof(arg) * 1000;
            }
        } else if (!strcasecmp(cmd, "AudioBitRate")) {
            get_arg(arg, sizeof(arg), &p);
            if (stream) {
                audio_enc.bit_rate = atoi(arg) * 1000;
            }
        } else if (!strcasecmp(cmd, "AudioChannels")) {
            get_arg(arg, sizeof(arg), &p);
            if (stream) {
                audio_enc.channels = atoi(arg);
            }
        } else if (!strcasecmp(cmd, "AudioSampleRate")) {
            get_arg(arg, sizeof(arg), &p);
            if (stream) {
                audio_enc.sample_rate = atoi(arg);
            }
	} else if (!strcasecmp(cmd, "AudioQuality")) {
	    get_arg(arg, sizeof(arg), &p);
            if (stream) {
//                audio_enc.quality = atof(arg) * 1000;
            }
        } else if (!strcasecmp(cmd, "VideoBitRateRange")) {
            if (stream) {
                int minrate, maxrate;

                get_arg(arg, sizeof(arg), &p);

                if (sscanf(arg, "%d-%d", &minrate, &maxrate) == 2) {
                    video_enc.rc_min_rate = minrate * 1000;
                    video_enc.rc_max_rate = maxrate * 1000;
                } else {
                    fprintf(stderr, "%s:%d: Incorrect format for VideoBitRateRange -- should be <min>-<max>: %s\n", 
                            filename, line_num, arg);
                    errors++;
                }
            }
        } else if (!strcasecmp(cmd, "Debug")) {
            if (stream) {
                get_arg(arg, sizeof(arg), &p);
                video_enc.debug = strtol(arg,0,0);
            }
        } else if (!strcasecmp(cmd, "Strict")) {
            if (stream) {
                get_arg(arg, sizeof(arg), &p);
                video_enc.strict_std_compliance = atoi(arg);
            }
        } else if (!strcasecmp(cmd, "VideoBufferSize")) {
            if (stream) {
                get_arg(arg, sizeof(arg), &p);
                video_enc.rc_buffer_size = atoi(arg) * 8*1024;
            }
        } else if (!strcasecmp(cmd, "VideoBitRateTolerance")) {
            if (stream) {
                get_arg(arg, sizeof(arg), &p);
                video_enc.bit_rate_tolerance = atoi(arg) * 1000;
            }
        } else if (!strcasecmp(cmd, "VideoBitRate")) {
            get_arg(arg, sizeof(arg), &p);
            if (stream) {
                video_enc.bit_rate = atoi(arg) * 1000;
            }
        } else if (!strcasecmp(cmd, "VideoSize")) {
            get_arg(arg, sizeof(arg), &p);
            if (stream) {
                parse_image_size(&video_enc.width, &video_enc.height, arg);
                if ((video_enc.width % 16) != 0 ||
                    (video_enc.height % 16) != 0) {
                    fprintf(stderr, "%s:%d: Image size must be a multiple of 16\n",
                            filename, line_num);
                    errors++;
                }
            }
        } else if (!strcasecmp(cmd, "VideoFrameRate")) {
            get_arg(arg, sizeof(arg), &p);
            if (stream) {
                video_enc.time_base.num= DEFAULT_FRAME_RATE_BASE;
                video_enc.time_base.den = (int)(strtod(arg, NULL) * video_enc.time_base.num);
            }
        } else if (!strcasecmp(cmd, "VideoGopSize")) {
            get_arg(arg, sizeof(arg), &p);
            if (stream) {
                video_enc.gop_size = atoi(arg);
            }
        } else if (!strcasecmp(cmd, "VideoIntraOnly")) {
            if (stream) {
                video_enc.gop_size = 1;
            }
        } else if (!strcasecmp(cmd, "VideoHighQuality")) {
            if (stream) {
                video_enc.mb_decision = FF_MB_DECISION_BITS;
            }
        } else if (!strcasecmp(cmd, "Video4MotionVector")) {
            if (stream) {
                video_enc.mb_decision = FF_MB_DECISION_BITS; //FIXME remove
                video_enc.flags |= CODEC_FLAG_4MV;
            }
        } else if (!strcasecmp(cmd, "VideoQDiff")) {
            get_arg(arg, sizeof(arg), &p);
            if (stream) {
                video_enc.max_qdiff = atoi(arg);
                if (video_enc.max_qdiff < 1 || video_enc.max_qdiff > 31) {
                    fprintf(stderr, "%s:%d: VideoQDiff out of range\n",
                            filename, line_num);
                    errors++;
                }
            }
        } else if (!strcasecmp(cmd, "VideoQMax")) {
            get_arg(arg, sizeof(arg), &p);
            if (stream) {
                video_enc.qmax = atoi(arg);
                if (video_enc.qmax < 1 || video_enc.qmax > 31) {
                    fprintf(stderr, "%s:%d: VideoQMax out of range\n",
                            filename, line_num);
                    errors++;
                }
            }
        } else if (!strcasecmp(cmd, "VideoQMin")) {
            get_arg(arg, sizeof(arg), &p);
            if (stream) {
                video_enc.qmin = atoi(arg);
                if (video_enc.qmin < 1 || video_enc.qmin > 31) {
                    fprintf(stderr, "%s:%d: VideoQMin out of range\n",
                            filename, line_num);
                    errors++;
                }
            }
        } else if (!strcasecmp(cmd, "LumaElim")) {
            get_arg(arg, sizeof(arg), &p);
            if (stream) {
                video_enc.luma_elim_threshold = atoi(arg);
            }
        } else if (!strcasecmp(cmd, "ChromaElim")) {
            get_arg(arg, sizeof(arg), &p);
            if (stream) {
                video_enc.chroma_elim_threshold = atoi(arg);
            }
        } else if (!strcasecmp(cmd, "LumiMask")) {
            get_arg(arg, sizeof(arg), &p);
            if (stream) {
                video_enc.lumi_masking = atof(arg);
            }
        } else if (!strcasecmp(cmd, "DarkMask")) {
            get_arg(arg, sizeof(arg), &p);
            if (stream) {
                video_enc.dark_masking = atof(arg);
            }
        } else if (!strcasecmp(cmd, "NoVideo")) {
            video_id = CODEC_ID_NONE;
        } else if (!strcasecmp(cmd, "NoAudio")) {
            audio_id = CODEC_ID_NONE;
        } else if (!strcasecmp(cmd, "ACL")) {
            IPAddressACL acl;
            struct hostent *he;

            get_arg(arg, sizeof(arg), &p);
            if (strcasecmp(arg, "allow") == 0) {
                acl.action = IP_ALLOW;
            } else if (strcasecmp(arg, "deny") == 0) {
                acl.action = IP_DENY;
            } else {
                fprintf(stderr, "%s:%d: ACL action '%s' is not ALLOW or DENY\n",
                        filename, line_num, arg);
                errors++;
            }

            get_arg(arg, sizeof(arg), &p);

            he = gethostbyname(arg);
            if (!he) {
                fprintf(stderr, "%s:%d: ACL refers to invalid host or ip address '%s'\n",
                        filename, line_num, arg);
                errors++;
            } else {
                /
                acl.first.s_addr = ntohl(((struct in_addr *) he->h_addr_list[0])->s_addr);
                acl.last = acl.first;
            }

            get_arg(arg, sizeof(arg), &p);

            if (arg[0]) {
                he = gethostbyname(arg);
                if (!he) {
                    fprintf(stderr, "%s:%d: ACL refers to invalid host or ip address '%s'\n",
                            filename, line_num, arg);
                    errors++;
                } else {
                    /
                    acl.last.s_addr = ntohl(((struct in_addr *) he->h_addr_list[0])->s_addr);
                }
            }

            if (!errors) {
                IPAddressACL *nacl = (IPAddressACL *) av_mallocz(sizeof(*nacl));
                IPAddressACL **naclp = 0;

                *nacl = acl;
                nacl->next = 0;

                if (stream) {
                    naclp = &stream->acl;
                } else if (feed) {
                    naclp = &feed->acl;
                } else {
                    fprintf(stderr, "%s:%d: ACL found not in <stream> or <feed>\n",
                            filename, line_num);
                    errors++;
                }

                if (naclp) {
                    while (*naclp)
                        naclp = &(*naclp)->next;

                    *naclp = nacl;
                }
            }
        } else if (!strcasecmp(cmd, "RTSPOption")) {
            get_arg(arg, sizeof(arg), &p);
            if (stream) {
                av_freep(&stream->rtsp_option);
                /
                stream->rtsp_option = av_malloc(strlen(arg) + 1);
                if (stream->rtsp_option) {
                    strcpy(stream->rtsp_option, arg);
                }
            }
        } else if (!strcasecmp(cmd, "MulticastAddress")) {
            get_arg(arg, sizeof(arg), &p);
            if (stream) {
                if (!inet_aton(arg, &stream->multicast_ip)) {
                    fprintf(stderr, "%s:%d: Invalid IP address: %s\n", 
                            filename, line_num, arg);
                    errors++;
                }
                stream->is_multicast = 1;
                stream->loop = 1; /
            }
        } else if (!strcasecmp(cmd, "MulticastPort")) {
            get_arg(arg, sizeof(arg), &p);
            if (stream) {
                stream->multicast_port = atoi(arg);
            }
        } else if (!strcasecmp(cmd, "MulticastTTL")) {
            get_arg(arg, sizeof(arg), &p);
            if (stream) {
                stream->multicast_ttl = atoi(arg);
            }
        } else if (!strcasecmp(cmd, "NoLoop")) {
            if (stream) {
                stream->loop = 0;
            }
        } else if (!strcasecmp(cmd, "</Stream>")) {
            if (!stream) {
                fprintf(stderr, "%s:%d: No corresponding <Stream> for </Stream>\n",
                        filename, line_num);
                errors++;
            }
            if (stream->feed && stream->fmt && strcmp(stream->fmt->name, "ffm") != 0) {
                if (audio_id != CODEC_ID_NONE) {
                    audio_enc.codec_type = CODEC_TYPE_AUDIO;
                    audio_enc.codec_id = audio_id;
                    add_codec(stream, &audio_enc);
                }
                if (video_id != CODEC_ID_NONE) {
                    video_enc.codec_type = CODEC_TYPE_VIDEO;
                    video_enc.codec_id = video_id;
                    add_codec(stream, &video_enc);
                }
            }
            stream = NULL;
        } else if (!strcasecmp(cmd, "<Redirect")) {
            /
            char *q;
            if (stream || feed || redirect) {
                fprintf(stderr, "%s:%d: Already in a tag\n",
                        filename, line_num);
                errors++;
            } else {
                redirect = av_mallocz(sizeof(FFStream));
                *last_stream = redirect;
                last_stream = &redirect->next;

                get_arg(redirect->filename, sizeof(redirect->filename), &p);
                q = strrchr(redirect->filename, '>');
                if (*q)
                    *q = '\0';
                redirect->stream_type = STREAM_TYPE_REDIRECT;
            }
        } else if (!strcasecmp(cmd, "URL")) {
            if (redirect) {
                get_arg(redirect->feed_filename, sizeof(redirect->feed_filename), &p);
            }
        } else if (!strcasecmp(cmd, "</Redirect>")) {
            if (!redirect) {
                fprintf(stderr, "%s:%d: No corresponding <Redirect> for </Redirect>\n",
                        filename, line_num);
                errors++;
            }
            if (!redirect->feed_filename[0]) {
                fprintf(stderr, "%s:%d: No URL found for <Redirect>\n",
                        filename, line_num);
                errors++;
            }
            redirect = NULL;
        } else if (!strcasecmp(cmd, "LoadModule")) {
            get_arg(arg, sizeof(arg), &p);
#ifdef CONFIG_HAVE_DLOPEN
            load_module(arg);
#else
            fprintf(stderr, "%s:%d: Module support not compiled into this version: '%s'\n", 
                    filename, line_num, arg);
            errors++;
#endif
        } else {
            fprintf(stderr, "%s:%d: Incorrect keyword: '%s'\n", 
                    filename, line_num, cmd);
            errors++;
        }
    }

    fclose(f);
    if (errors)
        return -1;
    else
        return 0;
}
