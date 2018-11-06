static int idcin_read_seek(AVFormatContext *s, int stream_index,
                           int64_t timestamp, int flags)
{
    IdcinDemuxContext *idcin = s->priv_data;

    if (idcin->first_pkt_pos > 0) {
        int ret = avio_seek(s->pb, idcin->first_pkt_pos, SEEK_SET);
        if (ret < 0)
            return ret;
        ff_update_cur_dts(s, s->streams[idcin->video_stream_index], 0);
        idcin->next_chunk_is_video = 1;
        idcin->current_audio_chunk = 0;
        return 0;
    }
    return -1;
}
