static void compute_status(HTTPContext *c)
{
    HTTPContext *c1;
    FFStream *stream;
    char *p;
    time_t ti;
    int i, len;
    AVIOContext *pb;

    if (avio_open_dyn_buf(&pb) < 0) {
        /
        c->buffer_ptr = c->buffer;
        c->buffer_end = c->buffer;
        return;
    }

    avio_printf(pb, "HTTP/1.0 200 OK\r\n");
    avio_printf(pb, "Content-type: %s\r\n", "text/html");
    avio_printf(pb, "Pragma: no-cache\r\n");
    avio_printf(pb, "\r\n");

    avio_printf(pb, "<html><head><title>%s Status</title>\n", program_name);
    if (c->stream->feed_filename[0])
        avio_printf(pb, "<link rel=\"shortcut icon\" href=\"%s\">\n", c->stream->feed_filename);
    avio_printf(pb, "</head>\n<body>");
    avio_printf(pb, "<h1>%s Status</h1>\n", program_name);
    /
    avio_printf(pb, "<h2>Available Streams</h2>\n");
    avio_printf(pb, "<table cellspacing=0 cellpadding=4>\n");
    avio_printf(pb, "<tr><th valign=top>Path<th align=left>Served<br>Conns<th><br>bytes<th valign=top>Format<th>Bit rate<br>kbits/s<th align=left>Video<br>kbits/s<th><br>Codec<th align=left>Audio<br>kbits/s<th><br>Codec<th align=left valign=top>Feed\n");
    stream = first_stream;
    while (stream != NULL) {
        char sfilename[1024];
        char *eosf;

        if (stream->feed != stream) {
            av_strlcpy(sfilename, stream->filename, sizeof(sfilename) - 10);
            eosf = sfilename + strlen(sfilename);
            if (eosf - sfilename >= 4) {
                if (strcmp(eosf - 4, ".asf") == 0)
                    strcpy(eosf - 4, ".asx");
                else if (strcmp(eosf - 3, ".rm") == 0)
                    strcpy(eosf - 3, ".ram");
                else if (stream->fmt && !strcmp(stream->fmt->name, "rtp")) {
                    /
                    eosf = strrchr(sfilename, '.');
                    if (!eosf)
                        eosf = sfilename + strlen(sfilename);
                    if (stream->is_multicast)
                        strcpy(eosf, ".sdp");
                    else
                        strcpy(eosf, ".rtsp");
                }
            }

            avio_printf(pb, "<tr><td><a href=\"/%s\">%s</a> ",
                         sfilename, stream->filename);
            avio_printf(pb, "<td align=right> %d <td align=right> ",
                        stream->conns_served);
            fmt_bytecount(pb, stream->bytes_served);
            switch(stream->stream_type) {
            case STREAM_TYPE_LIVE: {
                    int audio_bit_rate = 0;
                    int video_bit_rate = 0;
                    const char *audio_codec_name = "";
                    const char *video_codec_name = "";
                    const char *audio_codec_name_extra = "";
                    const char *video_codec_name_extra = "";

                    for(i=0;i<stream->nb_streams;i++) {
                        AVStream *st = stream->streams[i];
                        AVCodec *codec = avcodec_find_encoder(st->codec->codec_id);
                        switch(st->codec->codec_type) {
                        case AVMEDIA_TYPE_AUDIO:
                            audio_bit_rate += st->codec->bit_rate;
                            if (codec) {
                                if (*audio_codec_name)
                                    audio_codec_name_extra = "...";
                                audio_codec_name = codec->name;
                            }
                            break;
                        case AVMEDIA_TYPE_VIDEO:
                            video_bit_rate += st->codec->bit_rate;
                            if (codec) {
                                if (*video_codec_name)
                                    video_codec_name_extra = "...";
                                video_codec_name = codec->name;
                            }
                            break;
                        case AVMEDIA_TYPE_DATA:
                            video_bit_rate += st->codec->bit_rate;
                            break;
                        default:
                            abort();
                        }
                    }
                    avio_printf(pb, "<td align=center> %s <td align=right> %d <td align=right> %d <td> %s %s <td align=right> %d <td> %s %s",
                                 stream->fmt->name,
                                 stream->bandwidth,
                                 video_bit_rate / 1000, video_codec_name, video_codec_name_extra,
                                 audio_bit_rate / 1000, audio_codec_name, audio_codec_name_extra);
                    if (stream->feed)
                        avio_printf(pb, "<td>%s", stream->feed->filename);
                    else
                        avio_printf(pb, "<td>%s", stream->feed_filename);
                    avio_printf(pb, "\n");
                }
                break;
            default:
                avio_printf(pb, "<td align=center> - <td align=right> - <td align=right> - <td><td align=right> - <td>\n");
                break;
            }
        }
        stream = stream->next;
    }
    avio_printf(pb, "</table>\n");

    stream = first_stream;
    while (stream != NULL) {
        if (stream->feed == stream) {
            avio_printf(pb, "<h2>Feed %s</h2>", stream->filename);
            if (stream->pid) {
                avio_printf(pb, "Running as pid %d.\n", stream->pid);

#if defined(linux) && !defined(CONFIG_NOCUTILS)
                {
                    FILE *pid_stat;
                    char ps_cmd[64];

                    /
                    snprintf(ps_cmd, sizeof(ps_cmd),
                             "ps -o \"%%cpu,cputime\" --no-headers %d",
                             stream->pid);

                    pid_stat = popen(ps_cmd, "r");
                    if (pid_stat) {
                        char cpuperc[10];
                        char cpuused[64];

                        if (fscanf(pid_stat, "%10s %64s", cpuperc,
                                   cpuused) == 2) {
                            avio_printf(pb, "Currently using %s%% of the cpu. Total time used %s.\n",
                                         cpuperc, cpuused);
                        }
                        fclose(pid_stat);
                    }
                }
#endif

                avio_printf(pb, "<p>");
            }
            avio_printf(pb, "<table cellspacing=0 cellpadding=4><tr><th>Stream<th>type<th>kbits/s<th align=left>codec<th align=left>Parameters\n");

            for (i = 0; i < stream->nb_streams; i++) {
                AVStream *st = stream->streams[i];
                AVCodec *codec = avcodec_find_encoder(st->codec->codec_id);
                const char *type = "unknown";
                char parameters[64];

                parameters[0] = 0;

                switch(st->codec->codec_type) {
                case AVMEDIA_TYPE_AUDIO:
                    type = "audio";
                    snprintf(parameters, sizeof(parameters), "%d channel(s), %d Hz", st->codec->channels, st->codec->sample_rate);
                    break;
                case AVMEDIA_TYPE_VIDEO:
                    type = "video";
                    snprintf(parameters, sizeof(parameters), "%dx%d, q=%d-%d, fps=%d", st->codec->width, st->codec->height,
                                st->codec->qmin, st->codec->qmax, st->codec->time_base.den / st->codec->time_base.num);
                    break;
                default:
                    abort();
                }
                avio_printf(pb, "<tr><td align=right>%d<td>%s<td align=right>%d<td>%s<td>%s\n",
                        i, type, st->codec->bit_rate/1000, codec ? codec->name : "", parameters);
            }
            avio_printf(pb, "</table>\n");

        }
        stream = stream->next;
    }

    /
    avio_printf(pb, "<h2>Connection Status</h2>\n");

    avio_printf(pb, "Number of connections: %d / %d<br>\n",
                 nb_connections, nb_max_connections);

    avio_printf(pb, "Bandwidth in use: %"PRIu64"k / %"PRIu64"k<br>\n",
                 current_bandwidth, max_bandwidth);

    avio_printf(pb, "<table>\n");
    avio_printf(pb, "<tr><th>#<th>File<th>IP<th>Proto<th>State<th>Target bits/sec<th>Actual bits/sec<th>Bytes transferred\n");
    c1 = first_http_ctx;
    i = 0;
    while (c1 != NULL) {
        int bitrate;
        int j;

        bitrate = 0;
        if (c1->stream) {
            for (j = 0; j < c1->stream->nb_streams; j++) {
                if (!c1->stream->feed)
                    bitrate += c1->stream->streams[j]->codec->bit_rate;
                else if (c1->feed_streams[j] >= 0)
                    bitrate += c1->stream->feed->streams[c1->feed_streams[j]]->codec->bit_rate;
            }
        }

        i++;
        p = inet_ntoa(c1->from_addr.sin_addr);
        avio_printf(pb, "<tr><td><b>%d</b><td>%s%s<td>%s<td>%s<td>%s<td align=right>",
                    i,
                    c1->stream ? c1->stream->filename : "",
                    c1->state == HTTPSTATE_RECEIVE_DATA ? "(input)" : "",
                    p,
                    c1->protocol,
                    http_state[c1->state]);
        fmt_bytecount(pb, bitrate);
        avio_printf(pb, "<td align=right>");
        fmt_bytecount(pb, compute_datarate(&c1->datarate, c1->data_count) * 8);
        avio_printf(pb, "<td align=right>");
        fmt_bytecount(pb, c1->data_count);
        avio_printf(pb, "\n");
        c1 = c1->next;
    }
    avio_printf(pb, "</table>\n");

    /
    ti = time(NULL);
    p = ctime(&ti);
    avio_printf(pb, "<hr size=1 noshade>Generated at %s", p);
    avio_printf(pb, "</body>\n</html>\n");

    len = avio_close_dyn_buf(pb, &c->pb_buffer);
    c->buffer_ptr = c->pb_buffer;
    c->buffer_end = c->pb_buffer + len;
}
