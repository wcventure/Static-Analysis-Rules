static int rtp_write_packet(AVFormatContext *s1, AVPacket *pkt)
{
    RTPMuxContext *s = s1->priv_data;
    AVStream *st = s1->streams[0];
    int rtcp_bytes;
    int size= pkt->size;

    av_dlog(s1, "%d: write len=%d\n", pkt->stream_index, size);

    rtcp_bytes = ((s->octet_count - s->last_octet_count) * RTCP_TX_RATIO_NUM) /
        RTCP_TX_RATIO_DEN;
    if ((s->first_packet || ((rtcp_bytes >= RTCP_SR_SIZE) &&
                            (ff_ntp_time() - s->last_rtcp_ntp_time > 5000000))) &&
        !(s->flags & FF_RTP_FLAG_SKIP_RTCP)) {
        rtcp_send_sr(s1, ff_ntp_time(), 0);
        s->last_octet_count = s->octet_count;
        s->first_packet = 0;
    }
    s->cur_timestamp = s->base_timestamp + pkt->pts;

    switch(st->codec->codec_id) {
    case AV_CODEC_ID_PCM_MULAW:
    case AV_CODEC_ID_PCM_ALAW:
    case AV_CODEC_ID_PCM_U8:
    case AV_CODEC_ID_PCM_S8:
        return rtp_send_samples(s1, pkt->data, size, 8 * st->codec->channels);
    case AV_CODEC_ID_PCM_U16BE:
    case AV_CODEC_ID_PCM_U16LE:
    case AV_CODEC_ID_PCM_S16BE:
    case AV_CODEC_ID_PCM_S16LE:
        return rtp_send_samples(s1, pkt->data, size, 16 * st->codec->channels);
    case AV_CODEC_ID_ADPCM_G722:
        /
        return rtp_send_samples(s1, pkt->data, size, 8 * st->codec->channels);
    case AV_CODEC_ID_ADPCM_G726:
        return rtp_send_samples(s1, pkt->data, size,
                                st->codec->bits_per_coded_sample * st->codec->channels);
    case AV_CODEC_ID_MP2:
    case AV_CODEC_ID_MP3:
        rtp_send_mpegaudio(s1, pkt->data, size);
        break;
    case AV_CODEC_ID_MPEG1VIDEO:
    case AV_CODEC_ID_MPEG2VIDEO:
        ff_rtp_send_mpegvideo(s1, pkt->data, size);
        break;
    case AV_CODEC_ID_AAC:
        if (s->flags & FF_RTP_FLAG_MP4A_LATM)
            ff_rtp_send_latm(s1, pkt->data, size);
        else
            ff_rtp_send_aac(s1, pkt->data, size);
        break;
    case AV_CODEC_ID_AMR_NB:
    case AV_CODEC_ID_AMR_WB:
        ff_rtp_send_amr(s1, pkt->data, size);
        break;
    case AV_CODEC_ID_MPEG2TS:
        rtp_send_mpegts_raw(s1, pkt->data, size);
        break;
    case AV_CODEC_ID_H264:
        ff_rtp_send_h264(s1, pkt->data, size);
        break;
    case AV_CODEC_ID_H261:
        ff_rtp_send_h261(s1, pkt->data, size);
        break;
    case AV_CODEC_ID_H263:
        if (s->flags & FF_RTP_FLAG_RFC2190) {
            int mb_info_size = 0;
            const uint8_t *mb_info =
                av_packet_get_side_data(pkt, AV_PKT_DATA_H263_MB_INFO,
                                        &mb_info_size);
            ff_rtp_send_h263_rfc2190(s1, pkt->data, size, mb_info, mb_info_size);
            break;
        }
        /
    case AV_CODEC_ID_H263P:
        ff_rtp_send_h263(s1, pkt->data, size);
        break;
    case AV_CODEC_ID_HEVC:
        ff_rtp_send_hevc(s1, pkt->data, size);
        break;
    case AV_CODEC_ID_VORBIS:
    case AV_CODEC_ID_THEORA:
        ff_rtp_send_xiph(s1, pkt->data, size);
        break;
    case AV_CODEC_ID_VP8:
        ff_rtp_send_vp8(s1, pkt->data, size);
        break;
    case AV_CODEC_ID_ILBC:
        rtp_send_ilbc(s1, pkt->data, size);
        break;
    case AV_CODEC_ID_MJPEG:
        ff_rtp_send_jpeg(s1, pkt->data, size);
        break;
    case AV_CODEC_ID_OPUS:
        if (size > s->max_payload_size) {
            av_log(s1, AV_LOG_ERROR,
                   "Packet size %d too large for max RTP payload size %d\n",
                   size, s->max_payload_size);
            return AVERROR(EINVAL);
        }
        /
    default:
        /
        rtp_send_raw(s1, pkt->data, size);
        break;
    }
    return 0;
}
