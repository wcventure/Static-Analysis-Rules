static int guess_ni_flag(AVFormatContext *s){
    int i;
    int64_t last_start=0;
    int64_t first_end= INT64_MAX;
    int64_t oldpos= avio_tell(s->pb);
    int *idx;
    int64_t min_pos, pos;

    for(i=0; i<s->nb_streams; i++){
        AVStream *st = s->streams[i];
        int n= st->nb_index_entries;
        unsigned int size;

        if(n <= 0)
            continue;

        if(n >= 2){
            int64_t pos= st->index_entries[0].pos;
            avio_seek(s->pb, pos + 4, SEEK_SET);
            size= avio_rl32(s->pb);
            if(pos + size > st->index_entries[1].pos)
                last_start= INT64_MAX;
        }

        if(st->index_entries[0].pos > last_start)
            last_start= st->index_entries[0].pos;
        if(st->index_entries[n-1].pos < first_end)
            first_end= st->index_entries[n-1].pos;
    }
    avio_seek(s->pb, oldpos, SEEK_SET);
    if (last_start > first_end)
        return 1;
    idx= av_mallocz(sizeof(*idx) * s->nb_streams);
    for (min_pos=pos=0; min_pos!=INT64_MAX; pos= min_pos+1) {
        int64_t max_dts = INT64_MIN/2, min_dts= INT64_MAX/2;
        min_pos = INT64_MAX;

        for (i=0; i<s->nb_streams; i++) {
            AVStream *st = s->streams[i];
            int n= st->nb_index_entries;
            while (idx[i]<n && st->index_entries[idx[i]].pos < pos)
                idx[i]++;
            if (idx[i] < n) {
                min_dts = FFMIN(min_dts, av_rescale_q(st->index_entries[idx[i]].timestamp, st->time_base, AV_TIME_BASE_Q));
                min_pos = FFMIN(min_pos, st->index_entries[idx[i]].pos);
            }
            if (idx[i])
                max_dts = FFMAX(max_dts, av_rescale_q(st->index_entries[idx[i]-1].timestamp, st->time_base, AV_TIME_BASE_Q));
        }
        if(max_dts - min_dts > 2*AV_TIME_BASE) {
            av_free(idx);
            return 1;
        }
    }
    av_free(idx);
    return 0;
}
