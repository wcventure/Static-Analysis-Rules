static int asf_read_simple_index(AVFormatContext *s, const GUIDParseTable *g)
{
    ASFContext *asf = s->priv_data;
    AVIOContext *pb = s->pb;
    AVStream *st    = NULL;
    uint64_t interval; // index entry time interval in 100 ns units, usually it's 1s
    uint32_t pkt_num, nb_entries;
    int32_t prev_pkt_num = -1;
    int i, ret;
    uint64_t size = avio_rl64(pb);

    // simple index objects should be ordered by stream number, this loop tries to find
    // the first not indexed video stream
    for (i = 0; i < asf->nb_streams; i++) {
        if ((asf->asf_st[i]->type == AVMEDIA_TYPE_VIDEO) && !asf->asf_st[i]->indexed) {
            asf->asf_st[i]->indexed = 1;
            st = s->streams[asf->asf_st[i]->index];
            break;
        }
    }
    if (!st) {
        avio_skip(pb, size - 24); // if there's no video stream, skip index object
        return 0;
    }
    avio_skip(pb, 16); // skip File ID
    interval = avio_rl64(pb);
    avio_skip(pb, 4);
    nb_entries = avio_rl32(pb);
    for (i = 0; i < nb_entries; i++) {
        pkt_num = avio_rl32(pb);
        ret = avio_skip(pb, 2);
        if (ret < 0) {
            av_log(s, AV_LOG_ERROR, "Skipping failed in asf_read_simple_index.\n");
            return ret;
        }
        if (prev_pkt_num != pkt_num) {
            av_add_index_entry(st, asf->first_packet_offset + asf->packet_size *
                               pkt_num, av_rescale(interval, i, 10000),
                               asf->packet_size, 0, AVINDEX_KEYFRAME);
            prev_pkt_num = pkt_num;
        }
    }
    asf->is_simple_index = 1;
    align_position(pb, asf->offset, size);

    return 0;
}
