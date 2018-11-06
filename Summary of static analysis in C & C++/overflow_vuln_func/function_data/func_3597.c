static int hls_read(URLContext *h, uint8_t *buf, int size)
{
    HLSContext *s = h->priv_data;
    const char *url;
    int ret;
    int64_t reload_interval;

start:
    if (s->seg_hd) {
        ret = ffurl_read(s->seg_hd, buf, size);
        if (ret > 0)
            return ret;
    }
    if (s->seg_hd) {
        ffurl_close(s->seg_hd);
        s->seg_hd = NULL;
        s->cur_seq_no++;
    }
    reload_interval = s->n_segments > 0 ?
                      s->segments[s->n_segments - 1]->duration :
                      s->target_duration;
    reload_interval *= 1000000;
retry:
    if (!s->finished) {
        int64_t now = av_gettime();
        if (now - s->last_load_time >= reload_interval) {
            if ((ret = parse_playlist(h, s->playlisturl)) < 0)
                return ret;
            /
            reload_interval = s->target_duration * 500000;
        }
    }
    if (s->cur_seq_no < s->start_seq_no) {
        av_log(h, AV_LOG_WARNING,
               "skipping %d segments ahead, expired from playlist\n",
               s->start_seq_no - s->cur_seq_no);
        s->cur_seq_no = s->start_seq_no;
    }
    if (s->cur_seq_no - s->start_seq_no >= s->n_segments) {
        if (s->finished)
            return AVERROR_EOF;
        while (av_gettime() - s->last_load_time < reload_interval) {
            if (ff_check_interrupt(&h->interrupt_callback))
                return AVERROR_EXIT;
            av_usleep(100*1000);
        }
        goto retry;
    }
    url = s->segments[s->cur_seq_no - s->start_seq_no]->url,
    av_log(h, AV_LOG_DEBUG, "opening %s\n", url);
    ret = ffurl_open(&s->seg_hd, url, AVIO_FLAG_READ,
                     &h->interrupt_callback, NULL);
    if (ret < 0) {
        if (ff_check_interrupt(&h->interrupt_callback))
            return AVERROR_EXIT;
        av_log(h, AV_LOG_WARNING, "Unable to open %s\n", url);
        s->cur_seq_no++;
        goto retry;
    }
    goto start;
}
