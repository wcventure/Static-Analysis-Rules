static int write_manifest(AVFormatContext *s, int final)
{
    DASHContext *c = s->priv_data;
    AVIOContext *out;
    char temp_filename[1024];
    int ret, i;
    AVDictionaryEntry *title = av_dict_get(s->metadata, "title", NULL, 0);

    snprintf(temp_filename, sizeof(temp_filename), "%s.tmp", s->filename);
    ret = avio_open2(&out, temp_filename, AVIO_FLAG_WRITE, &s->interrupt_callback, NULL);
    if (ret < 0) {
        av_log(s, AV_LOG_ERROR, "Unable to open %s for writing\n", temp_filename);
        return ret;
    }
    avio_printf(out, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
    avio_printf(out, "<MPD xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
                "\txmlns=\"urn:mpeg:dash:schema:mpd:2011\"\n"
                "\txmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
                "\txsi:schemaLocation=\"urn:mpeg:DASH:schema:MPD:2011 http://standards.iso.org/ittf/PubliclyAvailableStandards/MPEG-DASH_schema_files/DASH-MPD.xsd\"\n"
                "\tprofiles=\"urn:mpeg:dash:profile:isoff-live:2011\"\n"
                "\ttype=\"%s\"\n", final ? "static" : "dynamic");
    if (final) {
        avio_printf(out, "\tmediaPresentationDuration=\"");
        write_time(out, c->total_duration);
        avio_printf(out, "\"\n");
    } else {
        int update_period = c->last_duration / AV_TIME_BASE;
        if (c->use_template && !c->use_timeline)
            update_period = 500;
        avio_printf(out, "\tminimumUpdatePeriod=\"PT%dS\"\n", update_period);
        avio_printf(out, "\tsuggestedPresentationDelay=\"PT%dS\"\n", c->last_duration / AV_TIME_BASE);
        if (!c->availability_start_time[0] && s->nb_streams > 0 && c->streams[0].nb_segments > 0) {
            time_t t = time(NULL);
            struct tm *ptm, tmbuf;
            ptm = gmtime_r(&t, &tmbuf);
            if (ptm) {
                if (!strftime(c->availability_start_time, sizeof(c->availability_start_time),
                              "%Y-%m-%dT%H:%M:%S", ptm))
                    c->availability_start_time[0] = '\0';
            }
        }
        if (c->availability_start_time[0])
            avio_printf(out, "\tavailabilityStartTime=\"%s\"\n", c->availability_start_time);
        if (c->window_size && c->use_template) {
            avio_printf(out, "\ttimeShiftBufferDepth=\"");
            write_time(out, c->last_duration * c->window_size);
            avio_printf(out, "\"\n");
        }
    }
    avio_printf(out, "\tminBufferTime=\"");
    write_time(out, c->last_duration);
    avio_printf(out, "\">\n");
    avio_printf(out, "\t<ProgramInformation>\n");
    if (title) {
        char *escaped = xmlescape(title->value);
        avio_printf(out, "\t\t<Title>%s</Title>\n", escaped);
        av_free(escaped);
    }
    avio_printf(out, "\t</ProgramInformation>\n");
    if (c->window_size && s->nb_streams > 0 && c->streams[0].nb_segments > 0 && !c->use_template) {
        OutputStream *os = &c->streams[0];
        int start_index = FFMAX(os->nb_segments - c->window_size, 0);
        int64_t start_time = av_rescale_q(os->segments[start_index]->time, s->streams[0]->time_base, AV_TIME_BASE_Q);
        avio_printf(out, "\t<Period start=\"");
        write_time(out, start_time);
        avio_printf(out, "\">\n");
    } else {
        avio_printf(out, "\t<Period start=\"PT0.0S\">\n");
    }

    if (c->has_video) {
        avio_printf(out, "\t\t<AdaptationSet id=\"video\" segmentAlignment=\"true\" bitstreamSwitching=\"true\">\n");
        for (i = 0; i < s->nb_streams; i++) {
            AVStream *st = s->streams[i];
            OutputStream *os = &c->streams[i];
            if (s->streams[i]->codec->codec_type != AVMEDIA_TYPE_VIDEO)
                continue;
            avio_printf(out, "\t\t\t<Representation id=\"%d\" mimeType=\"video/mp4\" codecs=\"%s\"%s width=\"%d\" height=\"%d\">\n", i, os->codec_str, os->bandwidth_str, st->codec->width, st->codec->height);
            output_segment_list(&c->streams[i], out, c);
            avio_printf(out, "\t\t\t</Representation>\n");
        }
        avio_printf(out, "\t\t</AdaptationSet>\n");
    }
    if (c->has_audio) {
        avio_printf(out, "\t\t<AdaptationSet id=\"audio\" segmentAlignment=\"true\" bitstreamSwitching=\"true\">\n");
        for (i = 0; i < s->nb_streams; i++) {
            AVStream *st = s->streams[i];
            OutputStream *os = &c->streams[i];
            if (s->streams[i]->codec->codec_type != AVMEDIA_TYPE_AUDIO)
                continue;
            avio_printf(out, "\t\t\t<Representation id=\"%d\" mimeType=\"audio/mp4\" codecs=\"%s\"%s audioSamplingRate=\"%d\">\n", i, os->codec_str, os->bandwidth_str, st->codec->sample_rate);
            avio_printf(out, "\t\t\t\t<AudioChannelConfiguration schemeIdUri=\"urn:mpeg:dash:23003:3:audio_channel_configuration:2011\" value=\"%d\" />\n", st->codec->channels);
            output_segment_list(&c->streams[i], out, c);
            avio_printf(out, "\t\t\t</Representation>\n");
        }
        avio_printf(out, "\t\t</AdaptationSet>\n");
    }
    avio_printf(out, "\t</Period>\n");
    avio_printf(out, "</MPD>\n");
    avio_flush(out);
    avio_close(out);
    return ff_rename(temp_filename, s->filename, s);
}
