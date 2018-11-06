#if CONFIG_SDP_DEMUXER
static int sdp_probe(AVProbeData *p1)
{
    const char *p = p1->buf, *p_end = p1->buf + p1->buf_size;

    /
    while (p < p_end && *p != '\0') {
        if (p + sizeof("c=IN IP") - 1 < p_end &&
            av_strstart(p, "c=IN IP", NULL))
            return AVPROBE_SCORE_EXTENSION;

        while (p < p_end - 1 && *p != '\n') p++;
        if (++p >= p_end)
            break;
        if (*p == '\r')
            p++;
    }
    return 0;
}
