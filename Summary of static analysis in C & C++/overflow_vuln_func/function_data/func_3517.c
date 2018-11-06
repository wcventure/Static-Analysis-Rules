static int rtp_write_header(AVFormatContext *s1)
{
    RTPMuxContext *s = s1->priv_data;
    int max_packet_size, n;
    AVStream *st;

    if (s1->nb_streams != 1)
        return -1;
    st = s1->streams[0];
    if (!is_supported(st->codec->codec_id)) {
        av_log(s1, AV_LOG_ERROR, "Unsupported codec %x\n", st->codec->codec_id);

        return -1;
    }

    if (s->payload_type < 0)
        s->payload_type = ff_rtp_get_payload_type(s1, st->codec);
    s->base_timestamp = av_get_random_seed();
    s->timestamp = s->base_timestamp;
    s->cur_timestamp = 0;
    s->ssrc = av_get_random_seed();
    s->first_packet = 1;
    s->first_rtcp_ntp_time = ff_ntp_time();
    if (s1->start_time_realtime)
        /
        s->first_rtcp_ntp_time = (s1->start_time_realtime / 1000) * 1000 +
                                 NTP_OFFSET_US;

    max_packet_size = s1->pb->max_packet_size;
    if (max_packet_size <= 12)
        return AVERROR(EIO);
    s->buf = av_malloc(max_packet_size);
    if (s->buf == NULL) {
        return AVERROR(ENOMEM);
    }
    s->max_payload_size = max_packet_size - 12;

    s->max_frames_per_packet = 0;
    if (s1->max_delay) {
        if (st->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (st->codec->frame_size == 0) {
                av_log(s1, AV_LOG_ERROR, "Cannot respect max delay: frame size = 0\n");
            } else {
                s->max_frames_per_packet = av_rescale_rnd(s1->max_delay, st->codec->sample_rate, AV_TIME_BASE * st->codec->frame_size, AV_ROUND_DOWN);
            }
        }
        if (st->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            /
            s->max_frames_per_packet = av_rescale_q(s1->max_delay, (AVRational){1, 1000000}, st->codec->time_base);
        }
    }

    avpriv_set_pts_info(st, 32, 1, 90000);
    switch(st->codec->codec_id) {
    case CODEC_ID_MP2:
    case CODEC_ID_MP3:
        s->buf_ptr = s->buf + 4;
        break;
    case CODEC_ID_MPEG1VIDEO:
    case CODEC_ID_MPEG2VIDEO:
        break;
    case CODEC_ID_MPEG2TS:
        n = s->max_payload_size / TS_PACKET_SIZE;
        if (n < 1)
            n = 1;
        s->max_payload_size = n * TS_PACKET_SIZE;
        s->buf_ptr = s->buf;
        break;
    case CODEC_ID_H264:
        /
        if (st->codec->extradata_size > 4 && st->codec->extradata[0] == 1) {
            s->nal_length_size = (st->codec->extradata[4] & 0x03) + 1;
        }
        break;
    case CODEC_ID_VORBIS:
    case CODEC_ID_THEORA:
        if (!s->max_frames_per_packet) s->max_frames_per_packet = 15;
        s->max_frames_per_packet = av_clip(s->max_frames_per_packet, 1, 15);
        s->max_payload_size -= 6; // ident+frag+tdt/vdt+pkt_num+pkt_length
        s->num_frames = 0;
        goto defaultcase;
    case CODEC_ID_VP8:
        av_log(s1, AV_LOG_ERROR, "RTP VP8 payload implementation is "
                                 "incompatible with the latest spec drafts.\n");
        break;
    case CODEC_ID_ADPCM_G722:
        /
        avpriv_set_pts_info(st, 32, 1, 8000);
        break;
    case CODEC_ID_AMR_NB:
    case CODEC_ID_AMR_WB:
        if (!s->max_frames_per_packet)
            s->max_frames_per_packet = 12;
        if (st->codec->codec_id == CODEC_ID_AMR_NB)
            n = 31;
        else
            n = 61;
        /
        if (1 + s->max_frames_per_packet + n > s->max_payload_size) {
            av_log(s1, AV_LOG_ERROR, "RTP max payload size too small for AMR\n");
            return -1;
        }
        if (st->codec->channels != 1) {
            av_log(s1, AV_LOG_ERROR, "Only mono is supported\n");
            return -1;
        }
    case CODEC_ID_AAC:
        s->num_frames = 0;
    default:
defaultcase:
        if (st->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            avpriv_set_pts_info(st, 32, 1, st->codec->sample_rate);
        }
        s->buf_ptr = s->buf;
        break;
    }

    return 0;
}
