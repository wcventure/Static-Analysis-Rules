static void compute_pkt_fields(AVFormatContext *s, AVStream *st,
                               AVCodecParserContext *pc, AVPacket *pkt)
{
    int num, den, presentation_delayed, delay, i;
    int64_t offset;

    if(pkt->pts != AV_NOPTS_VALUE && pkt->dts != AV_NOPTS_VALUE && pkt->dts > pkt->pts && st->pts_wrap_bits<63
       /){
        pkt->dts -= 1LL<<st->pts_wrap_bits;
    }

    if (pkt->duration == 0) {
        compute_frame_duration(&num, &den, st, pc, pkt);
        if (den && num) {
            pkt->duration = av_rescale(1, num * (int64_t)st->time_base.den, den * (int64_t)st->time_base.num);

            if(pkt->duration != 0 && s->packet_buffer)
                update_initial_durations(s, st, pkt);
        }
    }

    /
    if(pc && st->need_parsing == AVSTREAM_PARSE_TIMESTAMPS && pkt->size){
        /
        offset = av_rescale(pc->offset, pkt->duration, pkt->size);
        if(pkt->pts != AV_NOPTS_VALUE)
            pkt->pts += offset;
        if(pkt->dts != AV_NOPTS_VALUE)
            pkt->dts += offset;
    }

    /
    delay= st->codec->has_b_frames;
    presentation_delayed = 0;
    /
    if (delay &&
        pc && pc->pict_type != FF_B_TYPE)
        presentation_delayed = 1;
    /
    if(pkt->dts != AV_NOPTS_VALUE && pkt->pts != AV_NOPTS_VALUE && pkt->pts > pkt->dts)
        presentation_delayed = 1;

//    av_log(NULL, AV_LOG_DEBUG, "IN delayed:%d pts:%"PRId64", dts:%"PRId64" cur_dts:%"PRId64" st:%d pc:%p\n", presentation_delayed, pkt->pts, pkt->dts, st->cur_dts, pkt->stream_index, pc);
    /
    if(delay==0 || (delay==1 && pc)){
        if (presentation_delayed) {
            /
            /
            if (pkt->dts == AV_NOPTS_VALUE)
                pkt->dts = st->last_IP_pts;
            update_initial_timestamps(s, pkt->stream_index, pkt->dts, pkt->pts);
            if (pkt->dts == AV_NOPTS_VALUE)
                pkt->dts = st->cur_dts;

            /
            if (st->last_IP_duration == 0)
                st->last_IP_duration = pkt->duration;
            if(pkt->dts != AV_NOPTS_VALUE)
                st->cur_dts = pkt->dts + st->last_IP_duration;
            st->last_IP_duration  = pkt->duration;
            st->last_IP_pts= pkt->pts;
            /
        } else if(pkt->pts != AV_NOPTS_VALUE || pkt->dts != AV_NOPTS_VALUE || pkt->duration){
            if(pkt->pts != AV_NOPTS_VALUE && pkt->duration){
                int64_t old_diff= FFABS(st->cur_dts - pkt->duration - pkt->pts);
                int64_t new_diff= FFABS(st->cur_dts - pkt->pts);
                if(old_diff < new_diff && old_diff < (pkt->duration>>3)){
                    pkt->pts += pkt->duration;
    //                av_log(NULL, AV_LOG_DEBUG, "id:%d old:%"PRId64" new:%"PRId64" dur:%d cur:%"PRId64" size:%d\n", pkt->stream_index, old_diff, new_diff, pkt->duration, st->cur_dts, pkt->size);
                }
            }

            /
            if(pkt->pts == AV_NOPTS_VALUE)
                pkt->pts = pkt->dts;
            update_initial_timestamps(s, pkt->stream_index, pkt->pts, pkt->pts);
            if(pkt->pts == AV_NOPTS_VALUE)
                pkt->pts = st->cur_dts;
            pkt->dts = pkt->pts;
            if(pkt->pts != AV_NOPTS_VALUE)
                st->cur_dts = pkt->pts + pkt->duration;
        }
    }

    if(pkt->pts != AV_NOPTS_VALUE){
        st->pts_buffer[0]= pkt->pts;
        for(i=1; i<delay+1 && st->pts_buffer[i] == AV_NOPTS_VALUE; i++)
            st->pts_buffer[i]= (i-delay-1) * pkt->duration;
        for(i=0; i<delay && st->pts_buffer[i] > st->pts_buffer[i+1]; i++)
            FFSWAP(int64_t, st->pts_buffer[i], st->pts_buffer[i+1]);
        if(pkt->dts == AV_NOPTS_VALUE)
            pkt->dts= st->pts_buffer[0];
        if(delay>1){
            update_initial_timestamps(s, pkt->stream_index, pkt->dts, pkt->pts); // this should happen on the first packet
        }
        if(pkt->dts > st->cur_dts)
            st->cur_dts = pkt->dts;
    }

//    av_log(NULL, AV_LOG_ERROR, "OUTdelayed:%d/%d pts:%"PRId64", dts:%"PRId64" cur_dts:%"PRId64"\n", presentation_delayed, delay, pkt->pts, pkt->dts, st->cur_dts);

    /
    if(is_intra_only(st->codec))
        pkt->flags |= PKT_FLAG_KEY;
    else if (pc) {
        pkt->flags = 0;
        /
            if (pc->pict_type == FF_I_TYPE)
                pkt->flags |= PKT_FLAG_KEY;
    }
}
