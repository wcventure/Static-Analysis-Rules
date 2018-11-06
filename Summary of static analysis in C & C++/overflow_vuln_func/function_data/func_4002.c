static void flush_buffered(AVFormatContext *s1, int last)
{
    RTPMuxContext *s = s1->priv_data;
    if (s->buf_ptr != s->buf) {
        // If only sending one single NAL unit, skip the aggregation framing
        if (s->buffered_nals == 1)
            ff_rtp_send_data(s1, s->buf + 4, s->buf_ptr - s->buf - 4, last);
        else
            ff_rtp_send_data(s1, s->buf, s->buf_ptr - s->buf, last);
    }
    s->buf_ptr = s->buf;
    s->buffered_nals = 0;
}
