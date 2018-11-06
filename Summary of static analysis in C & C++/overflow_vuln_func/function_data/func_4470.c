static int asf_read_seek(AVFormatContext *s, int stream_index,
                         int64_t pts, int flags)
{
    ASFContext *asf = s->priv_data;
    AVStream *st    = s->streams[stream_index];
    int ret = 0;

    if (s->packet_size <= 0)
        return -1;

    /
    if (s->pb) {
        int ret = avio_seek_time(s->pb, stream_index, pts, flags);
        if (ret >= 0)
            asf_reset_header(s);
        if (ret != AVERROR(ENOSYS))
            return ret;
    }

    /
    if (!pts) {
        asf_reset_header(s);
        avio_seek(s->pb, s->internal->data_offset, SEEK_SET);
        return 0;
    }

    if (!asf->index_read) {
        ret = asf_build_simple_index(s, stream_index);
        if (ret < 0)
            asf->index_read = -1;
    }

    if (asf->index_read > 0 && st->index_entries) {
        int index = av_index_search_timestamp(st, pts, flags);
        if (index >= 0) {
            /
            uint64_t pos = st->index_entries[index].pos;

            /
            av_log(s, AV_LOG_DEBUG, "SEEKTO: %"PRId64"\n", pos);
            if(avio_seek(s->pb, pos, SEEK_SET) < 0)
                return -1;
            asf_reset_header(s);
            skip_to_key(s);
            return 0;
        }
    }
    /
    if (ff_seek_frame_binary(s, stream_index, pts, flags) < 0)
        return -1;
    asf_reset_header(s);
    skip_to_key(s);
    return 0;
}
