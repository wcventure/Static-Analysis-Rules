void av_dump_format(AVFormatContext *ic, int index,
                    const char *url, int is_output)
{
    int i;
    uint8_t *printed = ic->nb_streams ? av_mallocz(ic->nb_streams) : NULL;
    if (ic->nb_streams && !printed)
        return;

    av_log(NULL, AV_LOG_INFO, "%s #%d, %s, %s '%s':\n",
           is_output ? "Output" : "Input",
           index,
           is_output ? ic->oformat->name : ic->iformat->name,
           is_output ? "to" : "from", url);
    dump_metadata(NULL, ic->metadata, "  ");

    if (!is_output) {
        av_log(NULL, AV_LOG_INFO, "  Duration: ");
        if (ic->duration != AV_NOPTS_VALUE) {
            int hours, mins, secs, us;
            int64_t duration = ic->duration + 5000;
            secs  = duration / AV_TIME_BASE;
            us    = duration % AV_TIME_BASE;
            mins  = secs / 60;
            secs %= 60;
            hours = mins / 60;
            mins %= 60;
            av_log(NULL, AV_LOG_INFO, "%02d:%02d:%02d.%02d", hours, mins, secs,
                   (100 * us) / AV_TIME_BASE);
        } else {
            av_log(NULL, AV_LOG_INFO, "N/A");
        }
        if (ic->start_time != AV_NOPTS_VALUE) {
            int secs, us;
            av_log(NULL, AV_LOG_INFO, ", start: ");
            secs = ic->start_time / AV_TIME_BASE;
            us   = llabs(ic->start_time % AV_TIME_BASE);
            av_log(NULL, AV_LOG_INFO, "%d.%06d",
                   secs, (int) av_rescale(us, 1000000, AV_TIME_BASE));
        }
        av_log(NULL, AV_LOG_INFO, ", bitrate: ");
        if (ic->bit_rate)
            av_log(NULL, AV_LOG_INFO, "%"PRId64" kb/s", (int64_t)ic->bit_rate / 1000);
        else
            av_log(NULL, AV_LOG_INFO, "N/A");
        av_log(NULL, AV_LOG_INFO, "\n");
    }

    for (i = 0; i < ic->nb_chapters; i++) {
        AVChapter *ch = ic->chapters[i];
        av_log(NULL, AV_LOG_INFO, "    Chapter #%d:%d: ", index, i);
        av_log(NULL, AV_LOG_INFO,
               "start %f, ", ch->start * av_q2d(ch->time_base));
        av_log(NULL, AV_LOG_INFO,
               "end %f\n", ch->end * av_q2d(ch->time_base));

        dump_metadata(NULL, ch->metadata, "    ");
    }

    if (ic->nb_programs) {
        int j, k, total = 0;
        for (j = 0; j < ic->nb_programs; j++) {
            AVDictionaryEntry *name = av_dict_get(ic->programs[j]->metadata,
                                                  "name", NULL, 0);
            av_log(NULL, AV_LOG_INFO, "  Program %d %s\n", ic->programs[j]->id,
                   name ? name->value : "");
            dump_metadata(NULL, ic->programs[j]->metadata, "    ");
            for (k = 0; k < ic->programs[j]->nb_stream_indexes; k++) {
                dump_stream_format(ic, ic->programs[j]->stream_index[k],
                                   index, is_output);
                printed[ic->programs[j]->stream_index[k]] = 1;
            }
            total += ic->programs[j]->nb_stream_indexes;
        }
        if (total < ic->nb_streams)
            av_log(NULL, AV_LOG_INFO, "  No Program\n");
    }

    for (i = 0; i < ic->nb_streams; i++)
        if (!printed[i])
            dump_stream_format(ic, i, index, is_output);

    av_free(printed);
}
