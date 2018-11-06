static int rtcp_parse_packet(RTPDemuxContext *s, const unsigned char *buf, int len)
{
    if (buf[1] != 200)
        return -1;
    s->last_rtcp_ntp_time = AV_RB64(buf + 8);
    if (s->first_rtcp_ntp_time == AV_NOPTS_VALUE)
        s->first_rtcp_ntp_time = s->last_rtcp_ntp_time;
    s->last_rtcp_timestamp = AV_RB32(buf + 16);
    return 0;
}
