static void compute_stats(HTTPContext *c)
{
    HTTPContext *c1;
    FFStream *stream;
    char *q, *p;
    time_t ti;
    int i;

    q = c->buffer;
    q += sprintf(q, "HTTP/1.0 200 OK\r\n");
    q += sprintf(q, "Content-type: %s\r\n", "text/html");
    q += sprintf(q, "Pragma: no-cache\r\n");
    q += sprintf(q, "\r\n");
    
    q += sprintf(q, "<HEAD><TITLE>FFServer Status</TITLE></HEAD>\n<BODY>");
    q += sprintf(q, "<H1>FFServer Status</H1>\n");
    /
    q += sprintf(q, "<H2>Available Streams</H2>\n");
    q += sprintf(q, "<TABLE>\n");
    q += sprintf(q, "<TR><Th>Path<Th>Format<Th>Bit rate (kbits/s)<Th COLSPAN=2>Video<Th COLSPAN=2>Audio<Th align=left>Feed\n");
    stream = first_stream;
    while (stream != NULL) {
        char sfilename[1024];
        char *eosf;

        strlcpy(sfilename, stream->filename, sizeof(sfilename) - 1);
        eosf = sfilename + strlen(sfilename);
        if (eosf - sfilename >= 4) {
            if (strcmp(eosf - 4, ".asf") == 0) {
                strcpy(eosf - 4, ".asx");
            } else if (strcmp(eosf - 3, ".rm") == 0) {
                strcpy(eosf - 3, ".ram");
            }
        }
        
        q += sprintf(q, "<TR><TD><A HREF=\"/%s\">%s</A> ", 
                     sfilename, stream->filename);
        switch(stream->stream_type) {
        case STREAM_TYPE_LIVE:
            {
                int audio_bit_rate = 0;
                int video_bit_rate = 0;
                char *audio_codec_name = "";
                char *video_codec_name = "";
                char *audio_codec_name_extra = "";
                char *video_codec_name_extra = "";

                for(i=0;i<stream->nb_streams;i++) {
                    AVStream *st = stream->streams[i];
                    AVCodec *codec = avcodec_find_encoder(st->codec.codec_id);
                    switch(st->codec.codec_type) {
                    case CODEC_TYPE_AUDIO:
                        audio_bit_rate += st->codec.bit_rate;
                        if (codec) {
                            if (*audio_codec_name)
                                audio_codec_name_extra = "...";
                            audio_codec_name = codec->name;
                        }
                        break;
                    case CODEC_TYPE_VIDEO:
                        video_bit_rate += st->codec.bit_rate;
                        if (codec) {
                            if (*video_codec_name)
                                video_codec_name_extra = "...";
                            video_codec_name = codec->name;
                        }
                        break;
                    default:
                        abort();
                    }
                }
                q += sprintf(q, "<TD> %s <TD> %d <TD> %d <TD> %s %s <TD> %d <TD> %s %s", 
                             stream->fmt->name,
                             (audio_bit_rate + video_bit_rate) / 1000,
                             video_bit_rate / 1000, video_codec_name, video_codec_name_extra,
                             audio_bit_rate / 1000, audio_codec_name, audio_codec_name_extra);
                if (stream->feed) {
                    q += sprintf(q, "<TD>%s", stream->feed->filename);
                } else {
                    q += sprintf(q, "<TD>%s", stream->feed_filename);
                }
                q += sprintf(q, "\n");
            }
            break;
        default:
            q += sprintf(q, "<TD> - <TD> - <TD COLSPAN=2> - <TD COLSPAN=2> -\n");
            break;
        }
        stream = stream->next;
    }
    q += sprintf(q, "</TABLE>\n");
    
#if 0
                for(i=0;i<stream->nb_streams;i++) {
                    AVStream *st = stream->streams[i];
                    AVCodec *codec = avcodec_find_encoder(st->codec.codec_id);
                    switch(st->codec.codec_type) {
                    case CODEC_TYPE_AUDIO:
                        audio_bit_rate += st->codec.bit_rate;
                        if (codec) {
                            if (*audio_codec_name)
                                audio_codec_name_extra = "...";
                            audio_codec_name = codec->name;
                        }
                        break;
                    case CODEC_TYPE_VIDEO:
                        video_bit_rate += st->codec.bit_rate;
                        if (codec) {
                            if (*video_codec_name)
                                video_codec_name_extra = "...";
                            video_codec_name = codec->name;
                        }
                        break;
                    default:
                        abort();
                    }
                }
                q += sprintf(q, "<TD> %s <TD> %d <TD> %d <TD> %s %s <TD> %d <TD> %s %s", 
                             stream->fmt->name,
                             (audio_bit_rate + video_bit_rate) / 1000,
                             video_bit_rate / 1000, video_codec_name, video_codec_name_extra,
                             audio_bit_rate / 1000, audio_codec_name, audio_codec_name_extra);
                if (stream->feed) {
                    q += sprintf(q, "<TD>%s", stream->feed->filename);
                } else {
                    q += sprintf(q, "<TD>%s", stream->feed_filename);
                }
                q += sprintf(q, "\n");
            }
            break;
        default:
            q += sprintf(q, "<TD> - <TD> - <TD COLSPAN=2> - <TD COLSPAN=2> -\n");
            break;
        }
        stream = stream->next;
    }
    q += sprintf(q, "</TABLE>\n");
    
#if 0
    {
        float avg;
        AVCodecContext *enc;
        char buf[1024];
        
        /
        stream = first_feed;
        stream = first_feed;
        while (stream != NULL) {
            q += sprintf(q, "<H1>Feed '%s'</H1>\n", stream->filename);
            q += sprintf(q, "<TABLE>\n");
            q += sprintf(q, "<TR><TD>Parameters<TD>Frame count<TD>Size<TD>Avg bitrate (kbits/s)\n");
        stream = first_feed;
        while (stream != NULL) {
            q += sprintf(q, "<H1>Feed '%s'</H1>\n", stream->filename);
            q += sprintf(q, "<TABLE>\n");
            q += sprintf(q, "<TR><TD>Parameters<TD>Frame count<TD>Size<TD>Avg bitrate (kbits/s)\n");
            for(i=0;i<stream->nb_streams;i++) {
                AVStream *st = stream->streams[i];
                FeedData *fdata = st->priv_data;
                enc = &st->codec;
            
                avcodec_string(buf, sizeof(buf), enc);
                avg = fdata->avg_frame_size * (float)enc->rate * 8.0;
                if (enc->codec->type == CODEC_TYPE_AUDIO && enc->frame_size > 0)
                    avg /= enc->frame_size;
                q += sprintf(q, "<TR><TD>%s <TD> %d <TD> %Ld <TD> %0.1f\n", 
                             buf, enc->frame_number, fdata->data_count, avg / 1000.0);
            }
            q += sprintf(q, "</TABLE>\n");
            stream = stream->next_feed;
        }
    }
#endif

    /
    q += sprintf(q, "<H2>Connection Status</H2>\n");

    q += sprintf(q, "Number of connections: %d / %d<BR>\n",
                 nb_connections, nb_max_connections);

    q += sprintf(q, "Bandwidth in use: %dk / %dk<BR>\n",
                 nb_bandwidth, nb_max_bandwidth);

    q += sprintf(q, "<TABLE>\n");
    q += sprintf(q, "<TR><TD>#<TD>File<TD>IP<TD>State<TD>Size\n");
    c1 = first_http_ctx;
    i = 0;
    while (c1 != NULL) {
        i++;
        p = inet_ntoa(c1->from_addr.sin_addr);
        q += sprintf(q, "<TR><TD><B>%d</B><TD>%s%s <TD> %s <TD> %s <TD> %Ld\n", 
                     i, c1->stream->filename, 
                     c1->state == HTTPSTATE_RECEIVE_DATA ? "(input)" : "",
                     p, 
                     http_state[c1->state],
                     c1->data_count);
        c1 = c1->next;
    }
    q += sprintf(q, "</TABLE>\n");
    
    /
    ti = time(NULL);
    p = ctime(&ti);
    q += sprintf(q, "<HR>Generated at %s", p);
    q += sprintf(q, "</BODY>\n</HTML>\n");

    c->buffer_ptr = c->buffer;
    c->buffer_end = q;
}
