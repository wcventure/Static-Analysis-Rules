static int mpegts_write_packet_internal(AVFormatContext *s, AVPacket *pkt)
{
    AVStream *st = s->streams[pkt->stream_index];
    int size = pkt->size;
    uint8_t *buf= pkt->data;
    uint8_t *data= NULL;
    MpegTSWrite *ts = s->priv_data;
    MpegTSWriteStream *ts_st = st->priv_data;
    const int64_t delay = av_rescale(s->max_delay, 90000, AV_TIME_BASE)*2;
    int64_t dts = pkt->dts, pts = pkt->pts;

    if (ts->reemit_pat_pmt) {
        av_log(s, AV_LOG_WARNING, "resend_headers option is deprecated, use -mpegts_flags resend_headers\n");
        ts->reemit_pat_pmt = 0;
        ts->flags |= MPEGTS_FLAG_REEMIT_PAT_PMT;
    }

    if (ts->flags & MPEGTS_FLAG_REEMIT_PAT_PMT) {
        ts->pat_packet_count = ts->pat_packet_period - 1;
        ts->sdt_packet_count = ts->sdt_packet_period - 1;
        ts->flags &= ~MPEGTS_FLAG_REEMIT_PAT_PMT;
    }

    if(ts->copyts < 1){
        if (pts != AV_NOPTS_VALUE)
            pts += delay;
        if (dts != AV_NOPTS_VALUE)
            dts += delay;
    }

    if (ts_st->first_pts_check && pts == AV_NOPTS_VALUE) {
        av_log(s, AV_LOG_ERROR, "first pts value must set\n");
        return AVERROR_INVALIDDATA;
    }
    ts_st->first_pts_check = 0;

    if (st->codec->codec_id == AV_CODEC_ID_H264) {
        const uint8_t *p = buf, *buf_end = p+size;
        uint32_t state = -1;

        if (pkt->size < 5 || AV_RB32(pkt->data) != 0x0000001) {
            av_log(s, AV_LOG_ERROR, "H.264 bitstream malformed, "
                   "no startcode found, use the h264_mp4toannexb bitstream filter (-bsf h264_mp4toannexb)\n");
            return AVERROR(EINVAL);
        }

        do {
            p = avpriv_mpv_find_start_code(p, buf_end, &state);
            //av_log(s, AV_LOG_INFO, "nal %d\n", state & 0x1f);
        } while (p < buf_end && (state & 0x1f) != 9 &&
                 (state & 0x1f) != 5 && (state & 0x1f) != 1);

        if ((state & 0x1f) != 9) { // AUD NAL
            data = av_malloc(pkt->size+6);
            if (!data)
                return AVERROR(ENOMEM);
            memcpy(data+6, pkt->data, pkt->size);
            AV_WB32(data, 0x00000001);
            data[4] = 0x09;
            data[5] = 0xf0; // any slice type (0xe) + rbsp stop one bit
            buf  = data;
            size = pkt->size+6;
        }
    } else if (st->codec->codec_id == AV_CODEC_ID_AAC) {
        if (pkt->size < 2) {
            av_log(s, AV_LOG_ERROR, "AAC packet too short\n");
            return AVERROR_INVALIDDATA;
        }
        if ((AV_RB16(pkt->data) & 0xfff0) != 0xfff0) {
            int ret;
            AVPacket pkt2;

            if (!ts_st->amux) {
                av_log(s, AV_LOG_ERROR, "AAC bitstream not in ADTS format "
                       "and extradata missing\n");
                return AVERROR_INVALIDDATA;
            }

            av_init_packet(&pkt2);
            pkt2.data = pkt->data;
            pkt2.size = pkt->size;
            ret = avio_open_dyn_buf(&ts_st->amux->pb);
            if (ret < 0)
                return AVERROR(ENOMEM);

            ret = av_write_frame(ts_st->amux, &pkt2);
            if (ret < 0) {
                avio_close_dyn_buf(ts_st->amux->pb, &data);
                ts_st->amux->pb = NULL;
                av_free(data);
                return ret;
            }
            size = avio_close_dyn_buf(ts_st->amux->pb, &data);
            ts_st->amux->pb = NULL;
            buf = data;
        }
    }

    if (pkt->dts != AV_NOPTS_VALUE) {
        int i;
        for(i=0; i<s->nb_streams; i++){
            AVStream *st2 = s->streams[i];
            MpegTSWriteStream *ts_st2 = st2->priv_data;
            if(   ts_st2->payload_size
               && ts_st2->payload_dts == AV_NOPTS_VALUE || dts - ts_st2->payload_dts > delay/2){
                mpegts_write_pes(s, st2, ts_st2->payload, ts_st2->payload_size,
                                ts_st2->payload_pts, ts_st2->payload_dts,
                                ts_st2->payload_flags & AV_PKT_FLAG_KEY);
                ts_st2->payload_size = 0;
            }
        }
    }

    if (ts_st->payload_size && ts_st->payload_size + size > ts->pes_payload_size) {
        mpegts_write_pes(s, st, ts_st->payload, ts_st->payload_size,
                         ts_st->payload_pts, ts_st->payload_dts,
                         ts_st->payload_flags & AV_PKT_FLAG_KEY);
        ts_st->payload_size = 0;
    }

    if (st->codec->codec_type != AVMEDIA_TYPE_AUDIO || size > ts->pes_payload_size) {
        av_assert0(!ts_st->payload_size);
        // for video and subtitle, write a single pes packet
        mpegts_write_pes(s, st, buf, size, pts, dts, pkt->flags & AV_PKT_FLAG_KEY);
        av_free(data);
        return 0;
    }

    if (!ts_st->payload_size) {
        ts_st->payload_pts = pts;
        ts_st->payload_dts = dts;
        ts_st->payload_flags = pkt->flags;
    }

    memcpy(ts_st->payload + ts_st->payload_size, buf, size);
    ts_st->payload_size += size;

    av_free(data);

    return 0;
}
