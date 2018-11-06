static int http_parse_request(HTTPContext *c)
{
    char *p;
    int post;
    enum RedirType redir_type;
    char cmd[32];
    char info[1024], *filename;
    char url[1024], *q;
    char protocol[32];
    char msg[1024];
    const char *mime_type;
    FFStream *stream;
    int i;
    char ratebuf[32];
    char *useragent = 0;

    p = c->buffer;
    get_word(cmd, sizeof(cmd), (const char **)&p);
    pstrcpy(c->method, sizeof(c->method), cmd);

    if (!strcmp(cmd, "GET"))
        post = 0;
    else if (!strcmp(cmd, "POST"))
        post = 1;
    else
        return -1;

    get_word(url, sizeof(url), (const char **)&p);
    pstrcpy(c->url, sizeof(c->url), url);

    get_word(protocol, sizeof(protocol), (const char **)&p);
    if (strcmp(protocol, "HTTP/1.0") && strcmp(protocol, "HTTP/1.1"))
        return -1;

    pstrcpy(c->protocol, sizeof(c->protocol), protocol);
    
    /
    p = url;
    if (*p == '/')
        p++;
    filename = p;
    p = strchr(p, '?');
    if (p) {
        pstrcpy(info, sizeof(info), p);
        *p = '\0';
    } else {
        info[0] = '\0';
    }

    for (p = c->buffer; *p && *p != '\r' && *p != '\n'; ) {
        if (strncasecmp(p, "User-Agent:", 11) == 0) {
            useragent = p + 11;
            if (*useragent && *useragent != '\n' && isspace(*useragent))
                useragent++;
            break;
        }
        p = strchr(p, '\n');
        if (!p)
            break;

        p++;
    }

    redir_type = REDIR_NONE;
    if (match_ext(filename, "asx")) {
        redir_type = REDIR_ASX;
        filename[strlen(filename)-1] = 'f';
    } else if (match_ext(filename, "asf") &&
        (!useragent || strncasecmp(useragent, "NSPlayer", 8) != 0)) {
        /
        redir_type = REDIR_ASF;
    } else if (match_ext(filename, "rpm,ram")) {
        redir_type = REDIR_RAM;
        strcpy(filename + strlen(filename)-2, "m");
    } else if (match_ext(filename, "rtsp")) {
        redir_type = REDIR_RTSP;
        compute_real_filename(filename, sizeof(url) - 1);
    } else if (match_ext(filename, "sdp")) {
        redir_type = REDIR_SDP;
        compute_real_filename(filename, sizeof(url) - 1);
    }
    
    stream = first_stream;
    while (stream != NULL) {
        if (!strcmp(stream->filename, filename) && validate_acl(stream, c))
            break;
        stream = stream->next;
    }
    if (stream == NULL) {
        sprintf(msg, "File '%s' not found", url);
        goto send_error;
    }

    c->stream = stream;
    memcpy(c->feed_streams, stream->feed_streams, sizeof(c->feed_streams));
    memset(c->switch_feed_streams, -1, sizeof(c->switch_feed_streams));

    if (stream->stream_type == STREAM_TYPE_REDIRECT) {
        c->http_error = 301;
        q = c->buffer;
        q += sprintf(q, "HTTP/1.0 301 Moved\r\n");
        q += sprintf(q, "Location: %s\r\n", stream->feed_filename);
        q += sprintf(q, "Content-type: text/html\r\n");
        q += sprintf(q, "\r\n");
        q += sprintf(q, "<html><head><title>Moved</title></head><body>\r\n");
        q += sprintf(q, "You should be <a href=\"%s\">redirected</a>.\r\n", stream->feed_filename);
        q += sprintf(q, "</body></html>\r\n");

        /
        c->buffer_ptr = c->buffer;
        c->buffer_end = q;
        c->state = HTTPSTATE_SEND_HEADER;
        return 0;
    }

    /
    if (extract_rates(ratebuf, sizeof(ratebuf), c->buffer)) {
        if (modify_current_stream(c, ratebuf)) {
            for (i = 0; i < sizeof(c->feed_streams) / sizeof(c->feed_streams[0]); i++) {
                if (c->switch_feed_streams[i] >= 0)
                    do_switch_stream(c, i);
            }
        }
    }

    if (post == 0 && stream->stream_type == STREAM_TYPE_LIVE) {
        current_bandwidth += stream->bandwidth;
    }
    
    if (post == 0 && max_bandwidth < current_bandwidth) {
        c->http_error = 200;
        q = c->buffer;
        q += sprintf(q, "HTTP/1.0 200 Server too busy\r\n");
        q += sprintf(q, "Content-type: text/html\r\n");
        q += sprintf(q, "\r\n");
        q += sprintf(q, "<html><head><title>Too busy</title></head><body>\r\n");
        q += sprintf(q, "The server is too busy to serve your request at this time.<p>\r\n");
        q += sprintf(q, "The bandwidth being served (including your stream) is %dkbit/sec, and this exceeds the limit of %dkbit/sec\r\n",
            current_bandwidth, max_bandwidth);
        q += sprintf(q, "</body></html>\r\n");

        /
        c->buffer_ptr = c->buffer;
        c->buffer_end = q;
        c->state = HTTPSTATE_SEND_HEADER;
        return 0;
    }
    
    if (redir_type != REDIR_NONE) {
        char *hostinfo = 0;
        
        for (p = c->buffer; *p && *p != '\r' && *p != '\n'; ) {
            if (strncasecmp(p, "Host:", 5) == 0) {
                hostinfo = p + 5;
                break;
            }
            p = strchr(p, '\n');
            if (!p)
                break;

            p++;
        }

        if (hostinfo) {
            char *eoh;
            char hostbuf[260];

            while (isspace(*hostinfo))
                hostinfo++;

            eoh = strchr(hostinfo, '\n');
            if (eoh) {
                if (eoh[-1] == '\r')
                    eoh--;

                if (eoh - hostinfo < sizeof(hostbuf) - 1) {
                    memcpy(hostbuf, hostinfo, eoh - hostinfo);
                    hostbuf[eoh - hostinfo] = 0;

                    c->http_error = 200;
                    q = c->buffer;
                    switch(redir_type) {
                    case REDIR_ASX:
                        q += sprintf(q, "HTTP/1.0 200 ASX Follows\r\n");
                        q += sprintf(q, "Content-type: video/x-ms-asf\r\n");
                        q += sprintf(q, "\r\n");
                        q += sprintf(q, "<ASX Version=\"3\">\r\n");
                        q += sprintf(q, "<!-- Autogenerated by ffserver -->\r\n");
                        q += sprintf(q, "<ENTRY><REF HREF=\"http://%s/%s%s\"/></ENTRY>\r\n", 
                                hostbuf, filename, info);
                        q += sprintf(q, "</ASX>\r\n");
                        break;
                    case REDIR_RAM:
                        q += sprintf(q, "HTTP/1.0 200 RAM Follows\r\n");
                        q += sprintf(q, "Content-type: audio/x-pn-realaudio\r\n");
                        q += sprintf(q, "\r\n");
                        q += sprintf(q, "# Autogenerated by ffserver\r\n");
                        q += sprintf(q, "http://%s/%s%s\r\n", 
                                hostbuf, filename, info);
                        break;
                    case REDIR_ASF:
                        q += sprintf(q, "HTTP/1.0 200 ASF Redirect follows\r\n");
                        q += sprintf(q, "Content-type: video/x-ms-asf\r\n");
                        q += sprintf(q, "\r\n");
                        q += sprintf(q, "[Reference]\r\n");
                        q += sprintf(q, "Ref1=http://%s/%s%s\r\n", 
                                hostbuf, filename, info);
                        break;
                    case REDIR_RTSP:
                        {
                            char hostname[256], *p;
                            /
                            pstrcpy(hostname, sizeof(hostname), hostbuf);
                            p = strrchr(hostname, ':');
                            if (p)
                                *p = '\0';
                            q += sprintf(q, "HTTP/1.0 200 RTSP Redirect follows\r\n");
                            /
                            q += sprintf(q, "Content-type: application/x-rtsp\r\n");
                            q += sprintf(q, "\r\n");
                            q += sprintf(q, "rtsp://%s:%d/%s\r\n", 
                                         hostname, ntohs(my_rtsp_addr.sin_port), 
                                         filename);
                        }
                        break;
                    case REDIR_SDP:
                        {
                            uint8_t *sdp_data;
                            int sdp_data_size, len;
                            struct sockaddr_in my_addr;

                            q += sprintf(q, "HTTP/1.0 200 OK\r\n");
                            q += sprintf(q, "Content-type: application/sdp\r\n");
                            q += sprintf(q, "\r\n");

                            len = sizeof(my_addr);
                            getsockname(c->fd, (struct sockaddr *)&my_addr, &len);
                            
                            /
                            sdp_data_size = prepare_sdp_description(stream, 
                                                                    &sdp_data, 
                                                                    my_addr.sin_addr);
                            if (sdp_data_size > 0) {
                                memcpy(q, sdp_data, sdp_data_size);
                                q += sdp_data_size;
                                *q = '\0';
                                av_free(sdp_data);
                            }
                        }
                        break;
                    default:
                        av_abort();
                        break;
                    }

                    /
                    c->buffer_ptr = c->buffer;
                    c->buffer_end = q;
                    c->state = HTTPSTATE_SEND_HEADER;
                    return 0;
                }
            }
        }

        sprintf(msg, "ASX/RAM file not handled");
        goto send_error;
    }

    stream->conns_served++;

    /

    if (post) {
        /
        if (!stream->is_feed) {
            /
            char *logline = 0;
            int client_id = 0;
            
            for (p = c->buffer; *p && *p != '\r' && *p != '\n'; ) {
                if (strncasecmp(p, "Pragma: log-line=", 17) == 0) {
                    logline = p;
                    break;
                }
                if (strncasecmp(p, "Pragma: client-id=", 18) == 0) {
                    client_id = strtol(p + 18, 0, 10);
                }
                p = strchr(p, '\n');
                if (!p)
                    break;

                p++;
            }

            if (logline) {
                char *eol = strchr(logline, '\n');

                logline += 17;

                if (eol) {
                    if (eol[-1] == '\r')
                        eol--;
                    http_log("%.*s\n", eol - logline, logline);
                    c->suppress_log = 1;
                }
            }

#ifdef DEBUG_WMP
            http_log("\nGot request:\n%s\n", c->buffer);
#endif

            if (client_id && extract_rates(ratebuf, sizeof(ratebuf), c->buffer)) {
                HTTPContext *wmpc;

                /
                for (wmpc = first_http_ctx; wmpc; wmpc = wmpc->next) {
                    if (wmpc->wmp_client_id == client_id)
                        break;
                }

                if (wmpc) {
                    if (modify_current_stream(wmpc, ratebuf)) {
                        wmpc->switch_pending = 1;
                    }
                }
            }
            
            sprintf(msg, "POST command not handled");
            c->stream = 0;
            goto send_error;
        }
        if (http_start_receive_data(c) < 0) {
            sprintf(msg, "could not open feed");
            goto send_error;
        }
        c->http_error = 0;
        c->state = HTTPSTATE_RECEIVE_DATA;
        return 0;
    }

#ifdef DEBUG_WMP
    if (strcmp(stream->filename + strlen(stream->filename) - 4, ".asf") == 0) {
        http_log("\nGot request:\n%s\n", c->buffer);
    }
#endif

    if (c->stream->stream_type == STREAM_TYPE_STATUS)
        goto send_stats;

    /
    if (open_input_stream(c, info) < 0) {
        sprintf(msg, "Input stream corresponding to '%s' not found", url);
        goto send_error;
    }

    /
    q = c->buffer;
    q += sprintf(q, "HTTP/1.0 200 OK\r\n");
    mime_type = c->stream->fmt->mime_type;
    if (!mime_type)
        mime_type = "application/x-octet_stream";
    q += sprintf(q, "Pragma: no-cache\r\n");

    /
    if (!strcmp(c->stream->fmt->name,"asf_stream")) {
        /

        c->wmp_client_id = random() & 0x7fffffff;

        q += sprintf(q, "Server: Cougar 4.1.0.3923\r\nCache-Control: no-cache\r\nPragma: client-id=%d\r\nPragma: features=\"broadcast\"\r\n", c->wmp_client_id);
    }
    q += sprintf(q, "Content-Type: %s\r\n", mime_type);
    q += sprintf(q, "\r\n");
    
    /
    c->http_error = 0;
    c->buffer_ptr = c->buffer;
    c->buffer_end = q;
    c->state = HTTPSTATE_SEND_HEADER;
    return 0;
 send_error:
    c->http_error = 404;
    q = c->buffer;
    q += sprintf(q, "HTTP/1.0 404 Not Found\r\n");
    q += sprintf(q, "Content-type: %s\r\n", "text/html");
    q += sprintf(q, "\r\n");
    q += sprintf(q, "<HTML>\n");
    q += sprintf(q, "<HEAD><TITLE>404 Not Found</TITLE></HEAD>\n");
    q += sprintf(q, "<BODY>%s</BODY>\n", msg);
    q += sprintf(q, "</HTML>\n");

    /
    c->buffer_ptr = c->buffer;
    c->buffer_end = q;
    c->state = HTTPSTATE_SEND_HEADER;
    return 0;
 send_stats:
    compute_stats(c);
    c->http_error = 200; /
    c->state = HTTPSTATE_SEND_HEADER;
    return 0;
}
