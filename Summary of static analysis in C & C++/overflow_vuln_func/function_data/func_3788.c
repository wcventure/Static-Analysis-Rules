static int rtsp_parse_request(HTTPContext *c)
{
    const char *p, *p1, *p2;
    char cmd[32];
    char url[1024];
    char protocol[32];
    char line[1024];
    int len;
    RTSPMessageHeader header1, *header = &header1;

    c->buffer_ptr[0] = '\0';
    p = c->buffer;

    get_word(cmd, sizeof(cmd), &p);
    get_word(url, sizeof(url), &p);
    get_word(protocol, sizeof(protocol), &p);

    av_strlcpy(c->method, cmd, sizeof(c->method));
    av_strlcpy(c->url, url, sizeof(c->url));
    av_strlcpy(c->protocol, protocol, sizeof(c->protocol));

    if (url_open_dyn_buf(&c->pb) < 0) {
        /
        c->pb = NULL; /
        return -1;
    }

    /
    if (strcmp(protocol, "RTSP/1.0") != 0) {
        rtsp_reply_error(c, RTSP_STATUS_VERSION);
        goto the_end;
    }

    /
    memset(header, 0, sizeof(*header));
    /
    while (*p != '\n' && *p != '\0')
        p++;
    if (*p == '\n')
        p++;
    while (*p != '\0') {
        p1 = strchr(p, '\n');
        if (!p1)
            break;
        p2 = p1;
        if (p2 > p && p2[-1] == '\r')
            p2--;
        /
        if (p2 == p)
            break;
        len = p2 - p;
        if (len > sizeof(line) - 1)
            len = sizeof(line) - 1;
        memcpy(line, p, len);
        line[len] = '\0';
        ff_rtsp_parse_line(header, line, NULL);
        p = p1 + 1;
    }

    /
    c->seq = header->seq;

    if (!strcmp(cmd, "DESCRIBE"))
        rtsp_cmd_describe(c, url);
    else if (!strcmp(cmd, "OPTIONS"))
        rtsp_cmd_options(c, url);
    else if (!strcmp(cmd, "SETUP"))
        rtsp_cmd_setup(c, url, header);
    else if (!strcmp(cmd, "PLAY"))
        rtsp_cmd_play(c, url, header);
    else if (!strcmp(cmd, "PAUSE"))
        rtsp_cmd_pause(c, url, header);
    else if (!strcmp(cmd, "TEARDOWN"))
        rtsp_cmd_teardown(c, url, header);
    else
        rtsp_reply_error(c, RTSP_STATUS_METHOD);

 the_end:
    len = url_close_dyn_buf(c->pb, &c->pb_buffer);
    c->pb = NULL; /
    if (len < 0) {
        /
        return -1;
    }
    c->buffer_ptr = c->pb_buffer;
    c->buffer_end = c->pb_buffer + len;
    c->state = RTSPSTATE_SEND_REPLY;
    return 0;
}
