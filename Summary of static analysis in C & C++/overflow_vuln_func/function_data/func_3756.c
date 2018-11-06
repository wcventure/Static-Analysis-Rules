static int hds_write_packet(AVFormatContext *s, AVPacket *pkt)
{
    HDSContext *c = s->priv_data;
    AVStream *st = s->streams[pkt->stream_index];
    OutputStream *os = &c->streams[s->streams[pkt->stream_index]->id];
    int64_t end_dts = (os->fragment_index) * c->min_frag_duration;
    int ret;

    if (st->first_dts == AV_NOPTS_VALUE)
        st->first_dts = pkt->dts;

    if ((!os->has_video || st->codec->codec_type == AVMEDIA_TYPE_VIDEO) &&
        av_compare_ts(pkt->dts - st->first_dts, st->time_base,
                      end_dts, AV_TIME_BASE_Q) >= 0 &&
        pkt->flags & AV_PKT_FLAG_KEY && os->packets_written) {

        if ((ret = hds_flush(s, os, 0, pkt->dts)) < 0)
            return ret;
    }

    // Note, these fragment start timestamps, that represent a whole
    // OutputStream, assume all streams in it have the same time base.
    if (!os->packets_written)
        os->frag_start_ts = pkt->dts;
    os->last_ts = pkt->dts;

    os->packets_written++;
    return ff_write_chained(os->ctx, pkt->stream_index - os->first_stream, pkt, s);
}
