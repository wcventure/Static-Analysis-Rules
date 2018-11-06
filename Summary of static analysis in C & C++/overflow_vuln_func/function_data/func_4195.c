static int read_data(void *opaque, uint8_t *buf, int buf_size)
{
    struct variant *v = opaque;
    HLSContext *c = v->parent->priv_data;
    int ret, i;

restart:
    if (!v->input) {
        /
        int64_t reload_interval = v->n_segments > 0 ?
                                  v->segments[v->n_segments - 1]->duration :
                                  v->target_duration;
        reload_interval *= 1000000;

reload:
        if (!v->finished &&
            av_gettime() - v->last_load_time >= reload_interval) {
            if ((ret = parse_playlist(c, v->url, v, NULL)) < 0)
                return ret;
            /
            reload_interval = v->target_duration * 500000;
        }
        if (v->cur_seq_no < v->start_seq_no) {
            av_log(NULL, AV_LOG_WARNING,
                   "skipping %d segments ahead, expired from playlists\n",
                   v->start_seq_no - v->cur_seq_no);
            v->cur_seq_no = v->start_seq_no;
        }
        if (v->cur_seq_no >= v->start_seq_no + v->n_segments) {
            if (v->finished)
                return AVERROR_EOF;
            while (av_gettime() - v->last_load_time < reload_interval) {
                if (ff_check_interrupt(c->interrupt_callback))
                    return AVERROR_EXIT;
                av_usleep(100*1000);
            }
            /
            goto reload;
        }

        ret = open_input(v);
        if (ret < 0)
            return ret;
    }
    ret = ffurl_read(v->input, buf, buf_size);
    if (ret > 0)
        return ret;
    ffurl_close(v->input);
    v->input = NULL;
    v->cur_seq_no++;

    c->end_of_segment = 1;
    c->cur_seq_no = v->cur_seq_no;

    if (v->ctx && v->ctx->nb_streams && v->parent->nb_streams >= v->stream_offset + v->ctx->nb_streams) {
        v->needed = 0;
        for (i = v->stream_offset; i < v->stream_offset + v->ctx->nb_streams;
             i++) {
            if (v->parent->streams[i]->discard < AVDISCARD_ALL)
                v->needed = 1;
        }
    }
    if (!v->needed) {
        av_log(v->parent, AV_LOG_INFO, "No longer receiving variant %d\n",
               v->index);
        return AVERROR_EOF;
    }
    goto restart;
}
