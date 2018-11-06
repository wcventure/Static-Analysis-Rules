static int ism_write_packet(AVFormatContext *s, AVPacket *pkt)
{
    SmoothStreamingContext *c = s->priv_data;
    AVStream *st = s->streams[pkt->stream_index];
    OutputStream *os = &c->streams[pkt->stream_index];
    int64_t end_dts = (c->nb_fragments + 1) * c->min_frag_duration;
    int ret;

    if (st->first_dts == AV_NOPTS_VALUE)
        st->first_dts = pkt->dts;

    if ((!c->has_video || st->codec->codec_type == AVMEDIA_TYPE_VIDEO) &&
        av_compare_ts(pkt->dts - st->first_dts, st->time_base,
                      end_dts, AV_TIME_BASE_Q) >= 0 &&
        pkt->flags & AV_PKT_FLAG_KEY && os->packets_written) {

        if ((ret = ism_flush(s, 0)) < 0)
            return ret;
        c->nb_fragments++;
    }

    os->packets_written++;
    return ff_write_chained(os->ctx, 0, pkt, s);
}
