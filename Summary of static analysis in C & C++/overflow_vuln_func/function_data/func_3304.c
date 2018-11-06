static int handle_metadata(RTMPContext *rt, RTMPPacket *pkt)
{
    int ret, old_flv_size, type;
    const uint8_t *next;
    uint8_t *p;
    uint32_t size;
    uint32_t ts, cts, pts = 0;

    old_flv_size = update_offset(rt, pkt->size);

    if ((ret = av_reallocp(&rt->flv_data, rt->flv_size)) < 0) {
        rt->flv_size = rt->flv_off = 0;
        return ret;
    }

    next = pkt->data;
    p    = rt->flv_data + old_flv_size;

    /
    ts = pkt->timestamp;

    while (next - pkt->data < pkt->size - RTMP_HEADER) {
        type = bytestream_get_byte(&next);
        size = bytestream_get_be24(&next);
        cts  = bytestream_get_be24(&next);
        cts |= bytestream_get_byte(&next) << 24;
        if (!pts)
            pts = cts;
        ts += cts - pts;
        pts = cts;
        bytestream_put_byte(&p, type);
        bytestream_put_be24(&p, size);
        bytestream_put_be24(&p, ts);
        bytestream_put_byte(&p, ts >> 24);
        memcpy(p, next, size + 3 + 4);
        next += size + 3 + 4;
        p    += size + 3 + 4;
    }
    memcpy(p, next, RTMP_HEADER);

    return 0;
}
