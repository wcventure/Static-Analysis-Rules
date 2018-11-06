static int vp8_packet(AVFormatContext *s, int idx)
{
    struct ogg *ogg = s->priv_data;
    struct ogg_stream *os = ogg->streams + idx;
    uint8_t *p = os->buf + os->pstart;

    if ((!os->lastpts || os->lastpts == AV_NOPTS_VALUE) &&
        !(os->flags & OGG_FLAG_EOS)) {
        int seg;
        int duration;
        uint8_t *last_pkt = p;
        uint8_t *next_pkt;

        seg = os->segp;
        duration = (last_pkt[0] >> 4) & 1;
        next_pkt = last_pkt += os->psize;
        for (; seg < os->nsegs; seg++) {
            if (os->segments[seg] < 255) {
                duration += (last_pkt[0] >> 4) & 1;
                last_pkt  = next_pkt + os->segments[seg];
            }
            next_pkt += os->segments[seg];
        }
        os->lastpts =
        os->lastdts = vp8_gptopts(s, idx, os->granule, NULL) - duration;
        if(s->streams[idx]->start_time == AV_NOPTS_VALUE) {
            s->streams[idx]->start_time = os->lastpts;
            if (s->streams[idx]->duration)
                s->streams[idx]->duration -= s->streams[idx]->start_time;
        }
    }

    if (os->psize > 0)
        os->pduration = (p[0] >> 4) & 1;

    return 0;
}
