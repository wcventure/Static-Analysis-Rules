static int jacosub_read_header(AVFormatContext *s)
{
    AVBPrint header;
    AVIOContext *pb = s->pb;
    char line[JSS_MAX_LINESIZE];
    JACOsubContext *jacosub = s->priv_data;
    int shift_set = 0; // only the first shift matters
    int merge_line = 0;
    int i, ret;

    AVStream *st = avformat_new_stream(s, NULL);
    if (!st)
        return AVERROR(ENOMEM);
    avpriv_set_pts_info(st, 64, 1, 100);
    st->codec->codec_type = AVMEDIA_TYPE_SUBTITLE;
    st->codec->codec_id   = AV_CODEC_ID_JACOSUB;

    jacosub->timeres = 30;

    av_bprint_init(&header, 1024+FF_INPUT_BUFFER_PADDING_SIZE, 4096);

    while (!avio_feof(pb)) {
        int cmd_len;
        const char *p = line;
        int64_t pos = avio_tell(pb);
        int len = ff_get_line(pb, line, sizeof(line));

        p = jss_skip_whitespace(p);

        /
        if (merge_line || timed_line(p)) {
            AVPacket *sub;

            sub = ff_subtitles_queue_insert(&jacosub->q, line, len, merge_line);
            if (!sub)
                return AVERROR(ENOMEM);
            sub->pos = pos;
            merge_line = len > 1 && !strcmp(&line[len - 2], "\\\n");
            continue;
        }

        /
        if (*p != '#')
            continue;
        p++;
        i = get_jss_cmd(p[0]);
        if (i == -1)
            continue;

        /
        cmd_len = strlen(cmds[i]);
        if (av_strncasecmp(p, cmds[i], cmd_len) == 0)
            p += cmd_len;
        else
            p++;
        p = jss_skip_whitespace(p);

        /
        switch (cmds[i][0]) {
        case 'S': // SHIFT command affect the whole script...
            if (!shift_set) {
                jacosub->shift = get_shift(jacosub->timeres, p);
                shift_set = 1;
            }
            av_bprintf(&header, "#S %s", p);
            break;
        case 'T': // ...but must be placed after TIMERES
            jacosub->timeres = strtol(p, NULL, 10);
            if (!jacosub->timeres)
                jacosub->timeres = 30;
            else
                av_bprintf(&header, "#T %s", p);
            break;
        }
    }

    /
    ret = avpriv_bprint_to_extradata(st->codec, &header);
    if (ret < 0)
        return ret;

    /
    for (i = 0; i < jacosub->q.nb_subs; i++) {
        AVPacket *sub = &jacosub->q.subs[i];
        read_ts(jacosub, sub->data, &sub->pts, &sub->duration);
    }
    ff_subtitles_queue_finalize(&jacosub->q);

    return 0;
}
