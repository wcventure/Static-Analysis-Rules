static void compute_chapters_end(AVFormatContext *s)
{
    unsigned int i, j;
    int64_t max_time = 0;

    if (s->duration > 0)
        max_time = s->duration +
                       ((s->start_time == AV_NOPTS_VALUE) ? 0 : s->start_time);

    for (i = 0; i < s->nb_chapters; i++)
        if (s->chapters[i]->end == AV_NOPTS_VALUE) {
            AVChapter *ch = s->chapters[i];
            int64_t end = max_time ? av_rescale_q(max_time, AV_TIME_BASE_Q,
                                                  ch->time_base)
                                   : INT64_MAX;

            for (j = 0; j < s->nb_chapters; j++) {
                AVChapter *ch1     = s->chapters[j];
                int64_t next_start = av_rescale_q(ch1->start, ch1->time_base,
                                                  ch->time_base);
                if (j != i && next_start > ch->start && next_start < end)
                    end = next_start;
            }
            ch->end = (end == INT64_MAX) ? ch->start : end;
        }
}
