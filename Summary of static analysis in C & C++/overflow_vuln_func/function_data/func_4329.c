RTPDemuxContext *rtp_parse_open(AVFormatContext *s1, AVStream *st, URLContext *rtpc, int payload_type, RTPPayloadData *rtp_payload_data)
{
    RTPDemuxContext *s;

    s = av_mallocz(sizeof(RTPDemuxContext));
    if (!s)
        return NULL;
    s->payload_type = payload_type;
    s->last_rtcp_ntp_time = AV_NOPTS_VALUE;
    s->first_rtcp_ntp_time = AV_NOPTS_VALUE;
    s->ic = s1;
    s->st = st;
    s->rtp_payload_data = rtp_payload_data;
    rtp_init_statistics(&s->statistics, 0); // do we know the initial sequence from sdp?
    if (!strcmp(ff_rtp_enc_name(payload_type), "MP2T")) {
        s->ts = ff_mpegts_parse_open(s->ic);
        if (s->ts == NULL) {
            av_free(s);
            return NULL;
        }
    } else {
        av_set_pts_info(st, 32, 1, 90000);
        switch(st->codec->codec_id) {
        case CODEC_ID_MPEG1VIDEO:
        case CODEC_ID_MPEG2VIDEO:
        case CODEC_ID_MP2:
        case CODEC_ID_MP3:
        case CODEC_ID_MPEG4:
        case CODEC_ID_H263:
        case CODEC_ID_H264:
            st->need_parsing = AVSTREAM_PARSE_FULL;
            break;
        default:
            if (st->codec->codec_type == CODEC_TYPE_AUDIO) {
                av_set_pts_info(st, 32, 1, st->codec->sample_rate);
            }
            break;
        }
    }
    // needed to send back RTCP RR in RTSP sessions
    s->rtp_ctx = rtpc;
    gethostname(s->hostname, sizeof(s->hostname));
    return s;
}
