static int handle_packet(MpegTSContext *ts, const uint8_t *packet)
{
    MpegTSFilter *tss;
    int len, pid, cc, expected_cc, cc_ok, afc, is_start, is_discontinuity,
        has_adaptation, has_payload;
    const uint8_t *p, *p_end;
    int64_t pos;

    pid = AV_RB16(packet + 1) & 0x1fff;
    if (pid && discard_pid(ts, pid))
        return 0;
    is_start = packet[1] & 0x40;
    tss = ts->pids[pid];
    if (ts->auto_guess && !tss && is_start) {
        add_pes_stream(ts, pid, -1);
        tss = ts->pids[pid];
    }
    if (!tss)
        return 0;
    ts->current_pid = pid;

    afc = (packet[3] >> 4) & 3;
    if (afc == 0) /
        return 0;
    has_adaptation   = afc & 2;
    has_payload      = afc & 1;
    is_discontinuity = has_adaptation &&
                       packet[4] != 0 && /
                       (packet[5] & 0x80); /

    /
    cc = (packet[3] & 0xf);
    expected_cc = has_payload ? (tss->last_cc + 1) & 0x0f : tss->last_cc;
    cc_ok = pid == 0x1FFF || // null packet PID
            is_discontinuity ||
            tss->last_cc < 0 ||
            expected_cc == cc;

    tss->last_cc = cc;
    if (!cc_ok) {
        av_log(ts->stream, AV_LOG_DEBUG,
               "Continuity check failed for pid %d expected %d got %d\n",
               pid, expected_cc, cc);
        if (tss->type == MPEGTS_PES) {
            PESContext *pc = tss->u.pes_filter.opaque;
            pc->flags |= AV_PKT_FLAG_CORRUPT;
        }
    }

    p = packet + 4;
    if (has_adaptation) {
        int64_t pcr_h;
        int pcr_l;
        if (parse_pcr(&pcr_h, &pcr_l, packet) == 0)
            tss->last_pcr = pcr_h * 300 + pcr_l;
        /
        p += p[0] + 1;
    }
    /
    p_end = packet + TS_PACKET_SIZE;
    if (p >= p_end || !has_payload)
        return 0;

    pos = avio_tell(ts->stream->pb);
    if (pos >= 0) {
        av_assert0(pos >= TS_PACKET_SIZE);
        ts->pos47_full = pos - TS_PACKET_SIZE;
    }

    if (tss->type == MPEGTS_SECTION) {
        if (is_start) {
            /
            len = *p++;
            if (p + len > p_end)
                return 0;
            if (len && cc_ok) {
                /
                write_section_data(ts, tss,
                                   p, len, 0);
                /
                if (!ts->pids[pid])
                    return 0;
            }
            p += len;
            if (p < p_end) {
                write_section_data(ts, tss,
                                   p, p_end - p, 1);
            }
        } else {
            if (cc_ok) {
                write_section_data(ts, tss,
                                   p, p_end - p, 0);
            }
        }

        // stop find_stream_info from waiting for more streams
        // when all programs have received a PMT
        if (ts->stream->ctx_flags & AVFMTCTX_NOHEADER && ts->scan_all_pmts <= 0) {
            int i;
            for (i = 0; i < ts->nb_prg; i++) {
                if (!ts->prg[i].pmt_found)
                    break;
            }
            if (i == ts->nb_prg && ts->nb_prg > 0) {
                int types = 0;
                for (i = 0; i < ts->stream->nb_streams; i++) {
                    AVStream *st = ts->stream->streams[i];
                    types |= 1<<st->codec->codec_type;
                }
                if ((types & (1<<AVMEDIA_TYPE_AUDIO) && types & (1<<AVMEDIA_TYPE_VIDEO)) || pos > 100000) {
                    av_log(ts->stream, AV_LOG_DEBUG, "All programs have pmt, headers found\n");
                    ts->stream->ctx_flags &= ~AVFMTCTX_NOHEADER;
                }
            }
        }

    } else {
        int ret;
        // Note: The position here points actually behind the current packet.
        if (tss->type == MPEGTS_PES) {
            if ((ret = tss->u.pes_filter.pes_cb(tss, p, p_end - p, is_start,
                                                pos - ts->raw_packet_size)) < 0)
                return ret;
        }
    }

    return 0;
}
