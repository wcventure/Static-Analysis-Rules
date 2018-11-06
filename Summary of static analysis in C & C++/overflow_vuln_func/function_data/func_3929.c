int64_t ff_gen_search(AVFormatContext *s, int stream_index, int64_t target_ts,
                      int64_t pos_min, int64_t pos_max, int64_t pos_limit,
                      int64_t ts_min, int64_t ts_max, int flags, int64_t *ts_ret,
                      int64_t (*read_timestamp)(struct AVFormatContext *, int , int64_t *, int64_t ))
{
    int64_t pos, ts;
    int64_t start_pos, filesize;
    int no_change;

    av_dlog(s, "gen_seek: %d %s\n", stream_index, av_ts2str(target_ts));

    if(ts_min == AV_NOPTS_VALUE){
        pos_min = s->data_offset;
        ts_min = ff_read_timestamp(s, stream_index, &pos_min, INT64_MAX, read_timestamp);
        if (ts_min == AV_NOPTS_VALUE)
            return -1;
    }

    if(ts_min >= target_ts){
        *ts_ret= ts_min;
        return pos_min;
    }

    if(ts_max == AV_NOPTS_VALUE){
        int step= 1024;
        filesize = avio_size(s->pb);
        pos_max = filesize - 1;
        do{
            pos_max = FFMAX(0, pos_max - step);
            ts_max = ff_read_timestamp(s, stream_index, &pos_max, pos_max + step, read_timestamp);
            step += step;
        }while(ts_max == AV_NOPTS_VALUE && pos_max > 0);
        if (ts_max == AV_NOPTS_VALUE)
            return -1;

        for(;;){
            int64_t tmp_pos= pos_max + 1;
            int64_t tmp_ts= ff_read_timestamp(s, stream_index, &tmp_pos, INT64_MAX, read_timestamp);
            if(tmp_ts == AV_NOPTS_VALUE)
                break;
            ts_max= tmp_ts;
            pos_max= tmp_pos;
            if(tmp_pos >= filesize)
                break;
        }
        pos_limit= pos_max;
    }

    if(ts_max <= target_ts){
        *ts_ret= ts_max;
        return pos_max;
    }

    if(ts_min > ts_max){
        return -1;
    }else if(ts_min == ts_max){
        pos_limit= pos_min;
    }

    no_change=0;
    while (pos_min < pos_limit) {
        av_dlog(s, "pos_min=0x%"PRIx64" pos_max=0x%"PRIx64" dts_min=%s dts_max=%s\n",
                pos_min, pos_max, av_ts2str(ts_min), av_ts2str(ts_max));
        assert(pos_limit <= pos_max);

        if(no_change==0){
            int64_t approximate_keyframe_distance= pos_max - pos_limit;
            // interpolate position (better than dichotomy)
            pos = av_rescale(target_ts - ts_min, pos_max - pos_min, ts_max - ts_min)
                + pos_min - approximate_keyframe_distance;
        }else if(no_change==1){
            // bisection, if interpolation failed to change min or max pos last time
            pos = (pos_min + pos_limit)>>1;
        }else{
            /
            pos=pos_min;
        }
        if(pos <= pos_min)
            pos= pos_min + 1;
        else if(pos > pos_limit)
            pos= pos_limit;
        start_pos= pos;

        ts = ff_read_timestamp(s, stream_index, &pos, INT64_MAX, read_timestamp); //may pass pos_limit instead of -1
        if(pos == pos_max)
            no_change++;
        else
            no_change=0;
        av_dlog(s, "%"PRId64" %"PRId64" %"PRId64" / %s %s %s target:%s limit:%"PRId64" start:%"PRId64" noc:%d\n",
                pos_min, pos, pos_max,
                av_ts2str(ts_min), av_ts2str(ts), av_ts2str(ts_max), av_ts2str(target_ts),
                pos_limit, start_pos, no_change);
        if(ts == AV_NOPTS_VALUE){
            av_log(s, AV_LOG_ERROR, "read_timestamp() failed in the middle\n");
            return -1;
        }
        assert(ts != AV_NOPTS_VALUE);
        if (target_ts <= ts) {
            pos_limit = start_pos - 1;
            pos_max = pos;
            ts_max = ts;
        }
        if (target_ts >= ts) {
            pos_min = pos;
            ts_min = ts;
        }
    }

    pos = (flags & AVSEEK_FLAG_BACKWARD) ? pos_min : pos_max;
    ts  = (flags & AVSEEK_FLAG_BACKWARD) ?  ts_min :  ts_max;
#if 0
    pos_min = pos;
    ts_min = ff_read_timestamp(s, stream_index, &pos_min, INT64_MAX, read_timestamp);
    pos_min++;
    ts_max = ff_read_timestamp(s, stream_index, &pos_min, INT64_MAX, read_timestamp);
    av_dlog(s, "pos=0x%"PRIx64" %s<=%s<=%s\n",
            pos, av_ts2str(ts_min), av_ts2str(target_ts), av_ts2str(ts_max));
#endif
    *ts_ret= ts;
    return pos;
}
