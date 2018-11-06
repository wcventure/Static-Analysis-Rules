static int mv_read_packet(AVFormatContext *avctx, AVPacket *pkt)
{
    MvContext *mv = avctx->priv_data;
    AVIOContext *pb = avctx->pb;
    AVStream *st = avctx->streams[mv->stream_index];
    const AVIndexEntry *index;
    int frame = mv->frame[mv->stream_index];
    int ret;
    uint64_t pos;

    if (frame < st->nb_index_entries) {
        index = &st->index_entries[frame];
        pos   = avio_tell(pb);
        if (index->pos > pos)
            avio_skip(pb, index->pos - pos);
        else if (index->pos < pos) {
            if (!pb->seekable)
                return AVERROR(EIO);
            ret = avio_seek(pb, index->pos, SEEK_SET);
            if (ret < 0)
                return ret;
        }
        ret = av_get_packet(pb, pkt, index->size);
        if (ret < 0)
            return ret;

        pkt->stream_index = mv->stream_index;
        pkt->pts          = index->timestamp;
        pkt->flags       |= AV_PKT_FLAG_KEY;

        mv->frame[mv->stream_index]++;
        mv->eof_count = 0;
    } else {
        mv->eof_count++;
        if (mv->eof_count >= avctx->nb_streams)
            return AVERROR_EOF;

        // avoid returning 0 without a packet
        return AVERROR(EAGAIN);
    }

    mv->stream_index++;
    if (mv->stream_index >= avctx->nb_streams)
        mv->stream_index = 0;

    return 0;
}
