static int read_packet(AVFormatContext *s,
                       AVPacket *pkt)
{
    FilmstripDemuxContext *film = s->priv_data;
    AVStream *st = s->streams[0];

    if (s->pb->eof_reached)
        return AVERROR(EIO);
    pkt->dts = avio_tell(s->pb) / (st->codec->width * (st->codec->height + film->leading) * 4);
    pkt->size = av_get_packet(s->pb, pkt, st->codec->width * st->codec->height * 4);
    avio_skip(s->pb, st->codec->width * film->leading * 4);
    if (pkt->size < 0)
        return pkt->size;
    pkt->flags |= AV_PKT_FLAG_KEY;
    return 0;
}
