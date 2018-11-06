static int ogg_restore(AVFormatContext *s, int discard)
{
    struct ogg *ogg = s->priv_data;
    AVIOContext *bc = s->pb;
    struct ogg_state *ost = ogg->state;
    int i;

    if (!ost)
        return 0;

    ogg->state = ost->next;

    if (!discard){
        for (i = 0; i < ogg->nstreams; i++)
            av_free (ogg->streams[i].buf);

        avio_seek (bc, ost->pos, SEEK_SET);
        ogg->curidx = ost->curidx;
        ogg->nstreams = ost->nstreams;
        memcpy(ogg->streams, ost->streams,
               ost->nstreams * sizeof(*ogg->streams));
    }

    av_free (ost);

    return 0;
}
