static int get_packet(URLContext *s, int for_header)
{
    RTMPContext *rt = s->priv_data;
    int ret;
    uint8_t *p;
    const uint8_t *next;
    uint32_t data_size;
    uint32_t ts, cts, pts=0;

    if (rt->state == STATE_STOPPED)
        return AVERROR_EOF;

    for (;;) {
        RTMPPacket rpkt = { 0 };
        if ((ret = ff_rtmp_packet_read(rt->stream, &rpkt,
                                       rt->chunk_size, rt->prev_pkt[0])) <= 0) {
            if (ret == 0) {
                return AVERROR(EAGAIN);
            } else {
                return AVERROR(EIO);
            }
        }
        rt->bytes_read += ret;
        if (rt->bytes_read > rt->last_bytes_read + rt->client_report_size) {
            av_log(s, AV_LOG_DEBUG, "Sending bytes read report\n");
            gen_bytes_read(s, rt, rpkt.timestamp + 1);
            rt->last_bytes_read = rt->bytes_read;
        }

        ret = rtmp_parse_result(s, rt, &rpkt);
        if (ret < 0) {//serious error in current packet
            ff_rtmp_packet_destroy(&rpkt);
            return -1;
        }
        if (rt->state == STATE_STOPPED) {
            ff_rtmp_packet_destroy(&rpkt);
            return AVERROR_EOF;
        }
        if (for_header && (rt->state == STATE_PLAYING || rt->state == STATE_PUBLISHING)) {
            ff_rtmp_packet_destroy(&rpkt);
            return 0;
        }
        if (!rpkt.data_size || !rt->is_input) {
            ff_rtmp_packet_destroy(&rpkt);
            continue;
        }
        if (rpkt.type == RTMP_PT_VIDEO || rpkt.type == RTMP_PT_AUDIO ||
           (rpkt.type == RTMP_PT_NOTIFY && !memcmp("\002\000\012onMetaData", rpkt.data, 13))) {
            ts = rpkt.timestamp;

            // generate packet header and put data into buffer for FLV demuxer
            rt->flv_off  = 0;
            rt->flv_size = rpkt.data_size + 15;
            rt->flv_data = p = av_realloc(rt->flv_data, rt->flv_size);
            bytestream_put_byte(&p, rpkt.type);
            bytestream_put_be24(&p, rpkt.data_size);
            bytestream_put_be24(&p, ts);
            bytestream_put_byte(&p, ts >> 24);
            bytestream_put_be24(&p, 0);
            bytestream_put_buffer(&p, rpkt.data, rpkt.data_size);
            bytestream_put_be32(&p, 0);
            ff_rtmp_packet_destroy(&rpkt);
            return 0;
        } else if (rpkt.type == RTMP_PT_METADATA) {
            // we got raw FLV data, make it available for FLV demuxer
            rt->flv_off  = 0;
            rt->flv_size = rpkt.data_size;
            rt->flv_data = av_realloc(rt->flv_data, rt->flv_size);
            /
            next = rpkt.data;
            ts = rpkt.timestamp;
            while (next - rpkt.data < rpkt.data_size - 11) {
                next++;
                data_size = bytestream_get_be24(&next);
                p=next;
                cts = bytestream_get_be24(&next);
                cts |= bytestream_get_byte(&next) << 24;
                if (pts==0)
                    pts=cts;
                ts += cts - pts;
                pts = cts;
                bytestream_put_be24(&p, ts);
                bytestream_put_byte(&p, ts >> 24);
                next += data_size + 3 + 4;
            }
            memcpy(rt->flv_data, rpkt.data, rpkt.data_size);
            ff_rtmp_packet_destroy(&rpkt);
            return 0;
        }
        ff_rtmp_packet_destroy(&rpkt);
    }
}
