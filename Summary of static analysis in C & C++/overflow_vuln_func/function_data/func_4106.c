static void update_stream_timings(AVFormatContext *ic)
{
    int64_t start_time, start_time1, start_time_text, end_time, end_time1;
    int64_t duration, duration1, filesize;
    int i;
    AVStream *st;
    AVProgram *p;

    start_time = INT64_MAX;
    start_time_text = INT64_MAX;
    end_time = INT64_MIN;
    duration = INT64_MIN;
    for(i = 0;i < ic->nb_streams; i++) {
        st = ic->streams[i];
        if (st->start_time != AV_NOPTS_VALUE && st->time_base.den) {
            start_time1= av_rescale_q(st->start_time, st->time_base, AV_TIME_BASE_Q);
            if (st->codec->codec_type == AVMEDIA_TYPE_SUBTITLE || st->codec->codec_type == AVMEDIA_TYPE_DATA) {
                if (start_time1 < start_time_text)
                    start_time_text = start_time1;
            } else
                start_time = FFMIN(start_time, start_time1);
            end_time1 = AV_NOPTS_VALUE;
            if (st->duration != AV_NOPTS_VALUE) {
                end_time1 = start_time1
                          + av_rescale_q(st->duration, st->time_base, AV_TIME_BASE_Q);
                end_time = FFMAX(end_time, end_time1);
            }
            for(p = NULL; (p = av_find_program_from_stream(ic, p, i)); ){
                if(p->start_time == AV_NOPTS_VALUE || p->start_time > start_time1)
                    p->start_time = start_time1;
                if(p->end_time < end_time1)
                    p->end_time = end_time1;
            }
        }
        if (st->duration != AV_NOPTS_VALUE) {
            duration1 = av_rescale_q(st->duration, st->time_base, AV_TIME_BASE_Q);
            duration = FFMAX(duration, duration1);
        }
    }
    if (start_time == INT64_MAX || (start_time > start_time_text && start_time - start_time_text < AV_TIME_BASE))
        start_time = start_time_text;
    else if(start_time > start_time_text)
        av_log(ic, AV_LOG_VERBOSE, "Ignoring outlier non primary stream starttime %f\n", start_time_text / (float)AV_TIME_BASE);

    if (start_time != INT64_MAX) {
        ic->start_time = start_time;
        if (end_time != INT64_MIN) {
            if (ic->nb_programs) {
                for (i=0; i<ic->nb_programs; i++) {
                    p = ic->programs[i];
                    if(p->start_time != AV_NOPTS_VALUE && p->end_time > p->start_time)
                        duration = FFMAX(duration, p->end_time - p->start_time);
                }
            } else
                duration = FFMAX(duration, end_time - start_time);
        }
    }
    if (duration != INT64_MIN && duration > 0 && ic->duration == AV_NOPTS_VALUE) {
        ic->duration = duration;
    }
        if (ic->pb && (filesize = avio_size(ic->pb)) > 0 && ic->duration != AV_NOPTS_VALUE) {
            /
            ic->bit_rate = (double)filesize * 8.0 * AV_TIME_BASE /
                (double)ic->duration;
        }
}
