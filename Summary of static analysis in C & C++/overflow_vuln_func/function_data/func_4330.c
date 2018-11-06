static void finalize_packet(RTPDemuxContext *s, AVPacket *pkt, uint32_t timestamp)
{
    if (s->last_rtcp_ntp_time != AV_NOPTS_VALUE) {
        int64_t addend;
        int delta_timestamp;

        /
        delta_timestamp = timestamp - s->last_rtcp_timestamp;
        /
        addend = av_rescale(s->last_rtcp_ntp_time - s->first_rtcp_ntp_time, s->st->time_base.den, (uint64_t)s->st->time_base.num << 32);
        pkt->pts = addend + delta_timestamp;
    }
}
