static int mov_read_ctts(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    MOVStreamContext *sc;
    unsigned int i, entries;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams-1];
    sc = st->priv_data;

    avio_r8(pb); /
    avio_rb24(pb); /
    entries = avio_rb32(pb);

    av_log(c->fc, AV_LOG_TRACE, "track[%i].ctts.entries = %i\n", c->fc->nb_streams-1, entries);

    if (!entries)
        return 0;
    if (entries >= UINT_MAX / sizeof(*sc->ctts_data))
        return AVERROR_INVALIDDATA;
    av_freep(&sc->ctts_data);
    sc->ctts_data = av_realloc(NULL, entries * sizeof(*sc->ctts_data));
    if (!sc->ctts_data)
        return AVERROR(ENOMEM);

    for (i = 0; i < entries && !pb->eof_reached; i++) {
        int count    =avio_rb32(pb);
        int duration =avio_rb32(pb);

        sc->ctts_data[i].count   = count;
        sc->ctts_data[i].duration= duration;

        av_log(c->fc, AV_LOG_TRACE, "count=%d, duration=%d\n",
                count, duration);

        if (FFABS(duration) > (1<<28) && i+2<entries) {
            av_log(c->fc, AV_LOG_WARNING, "CTTS invalid\n");
            av_freep(&sc->ctts_data);
            sc->ctts_count = 0;
            return 0;
        }

        if (i+2<entries)
            mov_update_dts_shift(sc, duration);
    }

    sc->ctts_count = i;

    if (pb->eof_reached)
        return AVERROR_EOF;

    av_log(c->fc, AV_LOG_TRACE, "dts shift %d\n", sc->dts_shift);

    return 0;
}
