static void update_initial_timestamps(AVFormatContext *s, int stream_index,
                                      int64_t dts, int64_t pts, AVPacket *pkt)
{
    AVStream *st       = s->streams[stream_index];
    AVPacketList *pktl = s->internal->packet_buffer ? s->internal->packet_buffer : s->internal->parse_queue;
    int64_t pts_buffer[MAX_REORDER_DELAY+1];
    int64_t shift;
    int i, delay;

    if (st->first_dts != AV_NOPTS_VALUE ||
        dts           == AV_NOPTS_VALUE ||
        st->cur_dts   == AV_NOPTS_VALUE ||
        is_relative(dts))
        return;

    delay         = st->codec->has_b_frames;
    st->first_dts = dts - (st->cur_dts - RELATIVE_TS_BASE);
    st->cur_dts   = dts;
    shift         = st->first_dts - RELATIVE_TS_BASE;

    for (i = 0; i<MAX_REORDER_DELAY+1; i++)
        pts_buffer[i] = AV_NOPTS_VALUE;

    if (is_relative(pts))
        pts += shift;

    for (; pktl; pktl = get_next_pkt(s, st, pktl)) {
        if (pktl->pkt.stream_index != stream_index)
            continue;
        if (is_relative(pktl->pkt.pts))
            pktl->pkt.pts += shift;

        if (is_relative(pktl->pkt.dts))
            pktl->pkt.dts += shift;

        if (st->start_time == AV_NOPTS_VALUE && pktl->pkt.pts != AV_NOPTS_VALUE)
            st->start_time = pktl->pkt.pts;

        if (pktl->pkt.pts != AV_NOPTS_VALUE && delay <= MAX_REORDER_DELAY && has_decode_delay_been_guessed(st)) {
            pts_buffer[0] = pktl->pkt.pts;
            for (i = 0; i<delay && pts_buffer[i] > pts_buffer[i + 1]; i++)
                FFSWAP(int64_t, pts_buffer[i], pts_buffer[i + 1]);

            pktl->pkt.dts = select_from_pts_buffer(st, pts_buffer, pktl->pkt.dts);
        }
    }

    if (st->start_time == AV_NOPTS_VALUE)
        st->start_time = pts;
}
