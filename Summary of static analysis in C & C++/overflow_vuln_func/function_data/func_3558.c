static void rtcp_send_sr(AVFormatContext *s1, int64_t ntp_time, int bye)
{
    RTPMuxContext *s = s1->priv_data;
    uint32_t rtp_ts;

    av_log(s1, AV_LOG_TRACE, "RTCP: %02x %"PRIx64" %x\n", s->payload_type, ntp_time, s->timestamp);

    s->last_rtcp_ntp_time = ntp_time;
    rtp_ts = av_rescale_q(ntp_time - s->first_rtcp_ntp_time, (AVRational){1, 1000000},
                          s1->streams[0]->time_base) + s->base_timestamp;
    avio_w8(s1->pb, RTP_VERSION << 6);
    avio_w8(s1->pb, RTCP_SR);
    avio_wb16(s1->pb, 6); /
    avio_wb32(s1->pb, s->ssrc);
    avio_wb64(s1->pb, NTP_TO_RTP_FORMAT(ntp_time));
    avio_wb32(s1->pb, rtp_ts);
    avio_wb32(s1->pb, s->packet_count);
    avio_wb32(s1->pb, s->octet_count);

    if (s->cname) {
        int len = FFMIN(strlen(s->cname), 255);
        avio_w8(s1->pb, (RTP_VERSION << 6) + 1);
        avio_w8(s1->pb, RTCP_SDES);
        avio_wb16(s1->pb, (7 + len + 3) / 4); /

        avio_wb32(s1->pb, s->ssrc);
        avio_w8(s1->pb, 0x01); /
        avio_w8(s1->pb, len);
        avio_write(s1->pb, s->cname, len);
        avio_w8(s1->pb, 0); /
        for (len = (7 + len) % 4; len % 4; len++)
            avio_w8(s1->pb, 0);
    }

    if (bye) {
        avio_w8(s1->pb, (RTP_VERSION << 6) | 1);
        avio_w8(s1->pb, RTCP_BYE);
        avio_wb16(s1->pb, 1); /
        avio_wb32(s1->pb, s->ssrc);
    }

    avio_flush(s1->pb);
}
