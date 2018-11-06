static int parse_header(OutputStream *os, const uint8_t *buf, int buf_size)
{
    if (buf_size < 13)
        return AVERROR_INVALIDDATA;
    if (memcmp(buf, "FLV", 3))
        return AVERROR_INVALIDDATA;
    buf      += 13;
    buf_size -= 13;
    while (buf_size >= 11 + 4) {
        int type = buf[0];
        int size = AV_RB24(&buf[1]) + 11 + 4;
        if (size > buf_size)
            return AVERROR_INVALIDDATA;
        if (type == 8 || type == 9) {
            if (os->nb_extra_packets > FF_ARRAY_ELEMS(os->extra_packets))
                return AVERROR_INVALIDDATA;
            os->extra_packet_sizes[os->nb_extra_packets] = size;
            os->extra_packets[os->nb_extra_packets] = av_malloc(size);
            if (!os->extra_packets[os->nb_extra_packets])
                return AVERROR(ENOMEM);
            memcpy(os->extra_packets[os->nb_extra_packets], buf, size);
            os->nb_extra_packets++;
        } else if (type == 0x12) {
            if (os->metadata)
                return AVERROR_INVALIDDATA;
            os->metadata_size = size - 11 - 4;
            os->metadata      = av_malloc(os->metadata_size);
            if (!os->metadata)
                return AVERROR(ENOMEM);
            memcpy(os->metadata, buf + 11, os->metadata_size);
        }
        buf      += size;
        buf_size -= size;
    }
    if (!os->metadata)
        return AVERROR_INVALIDDATA;
    return 0;
}
