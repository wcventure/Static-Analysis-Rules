static void compute_pkt_fields(AVFormatContext *s, AVStream *st,
                               AVCodecParserContext *pc, AVPacket *pkt,
                               int64_t next_dts, int64_t next_pts)
{
    int num, den, presentation_delayed, delay, i;
    int64_t offset;
    AVRational duration;
    int onein_oneout = st->codec->codec_id != AV_CODEC_ID_H264 &&
                       st->codec->codec_id != AV_CODEC_ID_HEVC;

    if (s->flags & AVFMT_FLAG_NOFILLIN)
        return;

    if (st->codec->codec_type == AVMEDIA_TYPE_VIDEO && pkt->dts != AV_NOPTS_VALUE) {
        if (pkt->dts == pkt->pts && st->last_dts_for_order_check != AV_NOPTS_VALUE) {
            if (st->last_dts_for_order_check <= pkt->dts) {
                st->dts_ordered++;
            } else {
                av_log(s, st->dts_misordered ? AV_LOG_DEBUG : AV_LOG_WARNING,
                       "DTS %"PRIi64" < %"PRIi64" out of order\n",
                       pkt->dts,
                       st->last_dts_for_order_check);
                st->dts_misordered++;
            }
            if (st->dts_ordered + st->dts_misordered > 250) {
                st->dts_ordered    >>= 1;
                st->dts_misordered >>= 1;
            }
        }

        st->last_dts_for_order_check = pkt->dts;
        if (st->dts_ordered < 8*st->dts_misordered && pkt->dts == pkt->pts)
            pkt->dts = AV_NOPTS_VALUE;
    }

    if ((s->flags & AVFMT_FLAG_IGNDTS) && pkt->pts != AV_NOPTS_VALUE)
        pkt->dts = AV_NOPTS_VALUE;

    if (pc && pc->pict_type == AV_PICTURE_TYPE_B
        && !st->codec->has_b_frames)
        //FIXME Set low_delay = 0 when has_b_frames = 1
        st->codec->has_b_frames = 1;

    /
    delay = st->codec->has_b_frames;
    presentation_delayed = 0;

    /
    if (delay &&
        pc && pc->pict_type != AV_PICTURE_TYPE_B)
        presentation_delayed = 1;

    if (pkt->pts != AV_NOPTS_VALUE && pkt->dts != AV_NOPTS_VALUE &&
        st->pts_wrap_bits < 63 &&
        pkt->dts - (1LL << (st->pts_wrap_bits - 1)) > pkt->pts) {
        if (is_relative(st->cur_dts) || pkt->dts - (1LL<<(st->pts_wrap_bits - 1)) > st->cur_dts) {
            pkt->dts -= 1LL << st->pts_wrap_bits;
        } else
            pkt->pts += 1LL << st->pts_wrap_bits;
    }

    /
    if (delay == 1 && pkt->dts == pkt->pts &&
        pkt->dts != AV_NOPTS_VALUE && presentation_delayed) {
        av_log(s, AV_LOG_DEBUG, "invalid dts/pts combination %"PRIi64"\n", pkt->dts);
        if (    strcmp(s->iformat->name, "mov,mp4,m4a,3gp,3g2,mj2")
             && strcmp(s->iformat->name, "flv")) // otherwise we discard correct timestamps for vc1-wmapro.ism
            pkt->dts = AV_NOPTS_VALUE;
    }

    duration = av_mul_q((AVRational) {pkt->duration, 1}, st->time_base);
    if (pkt->duration == 0) {
        ff_compute_frame_duration(s, &num, &den, st, pc, pkt);
        if (den && num) {
            duration = (AVRational) {num, den};
            pkt->duration = av_rescale_rnd(1,
                                           num * (int64_t) st->time_base.den,
                                           den * (int64_t) st->time_base.num,
                                           AV_ROUND_DOWN);
        }
    }

    if (pkt->duration != 0 && (s->packet_buffer || s->parse_queue))
        update_initial_durations(s, st, pkt->stream_index, pkt->duration);

    /
    if (pc && st->need_parsing == AVSTREAM_PARSE_TIMESTAMPS && pkt->size) {
        /
        offset = av_rescale(pc->offset, pkt->duration, pkt->size);
        if (pkt->pts != AV_NOPTS_VALUE)
            pkt->pts += offset;
        if (pkt->dts != AV_NOPTS_VALUE)
            pkt->dts += offset;
    }

    /
    if (pkt->dts != AV_NOPTS_VALUE &&
        pkt->pts != AV_NOPTS_VALUE &&
        pkt->pts > pkt->dts)
        presentation_delayed = 1;

    av_dlog(NULL,
            "IN delayed:%d pts:%s, dts:%s cur_dts:%s st:%d pc:%p duration:%d delay:%d onein_oneout:%d\n",
            presentation_delayed, av_ts2str(pkt->pts), av_ts2str(pkt->dts), av_ts2str(st->cur_dts),
            pkt->stream_index, pc, pkt->duration, delay, onein_oneout);
    /
    if ((delay == 0 || (delay == 1 && pc)) &&
        onein_oneout) {
        if (presentation_delayed) {
            /
            /
            if (pkt->dts == AV_NOPTS_VALUE)
                pkt->dts = st->last_IP_pts;
            update_initial_timestamps(s, pkt->stream_index, pkt->dts, pkt->pts, pkt);
            if (pkt->dts == AV_NOPTS_VALUE)
                pkt->dts = st->cur_dts;

            /
            if (st->last_IP_duration == 0)
                st->last_IP_duration = pkt->duration;
            if (pkt->dts != AV_NOPTS_VALUE)
                st->cur_dts = pkt->dts + st->last_IP_duration;
            if (pkt->dts != AV_NOPTS_VALUE &&
                pkt->pts == AV_NOPTS_VALUE &&
                st->last_IP_duration > 0 &&
                (st->cur_dts - next_dts) <= 1 &&
                next_dts != next_pts &&
                next_pts != AV_NOPTS_VALUE)
                pkt->pts = next_dts;

            st->last_IP_duration = pkt->duration;
            st->last_IP_pts      = pkt->pts;
            /
        } else if (pkt->pts != AV_NOPTS_VALUE ||
                   pkt->dts != AV_NOPTS_VALUE ||
                   pkt->duration                ) {

            /
            if (pkt->pts == AV_NOPTS_VALUE)
                pkt->pts = pkt->dts;
            update_initial_timestamps(s, pkt->stream_index, pkt->pts,
                                      pkt->pts, pkt);
            if (pkt->pts == AV_NOPTS_VALUE)
                pkt->pts = st->cur_dts;
            pkt->dts = pkt->pts;
            if (pkt->pts != AV_NOPTS_VALUE)
                st->cur_dts = av_add_stable(st->time_base, pkt->pts, duration, 1);
        }
    }

    if (pkt->pts != AV_NOPTS_VALUE && delay <= MAX_REORDER_DELAY && has_decode_delay_been_guessed(st)) {
        st->pts_buffer[0] = pkt->pts;
        for (i = 0; i<delay && st->pts_buffer[i] > st->pts_buffer[i + 1]; i++)
            FFSWAP(int64_t, st->pts_buffer[i], st->pts_buffer[i + 1]);

        pkt->dts = select_from_pts_buffer(st, st->pts_buffer, pkt->dts);
    }
    // We skipped it above so we try here.
    if (!onein_oneout)
        // This should happen on the first packet
        update_initial_timestamps(s, pkt->stream_index, pkt->dts, pkt->pts, pkt);
    if (pkt->dts > st->cur_dts)
        st->cur_dts = pkt->dts;

    av_dlog(NULL, "OUTdelayed:%d/%d pts:%s, dts:%s cur_dts:%s\n",
            presentation_delayed, delay, av_ts2str(pkt->pts), av_ts2str(pkt->dts), av_ts2str(st->cur_dts));

    /
    if (is_intra_only(st->codec))
        pkt->flags |= AV_PKT_FLAG_KEY;
    if (pc)
        pkt->convergence_duration = pc->convergence_duration;
}
