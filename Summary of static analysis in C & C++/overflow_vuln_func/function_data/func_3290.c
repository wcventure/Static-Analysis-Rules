static int read_header(AVFormatContext *s)
{
    WtvContext *wtv = s->priv_data;
    int root_sector, root_size;
    uint8_t root[WTV_SECTOR_SIZE];
    AVIOContext *pb;
    int64_t timeline_pos;
    int ret;

    wtv->epoch          =
    wtv->pts            =
    wtv->last_valid_pts = AV_NOPTS_VALUE;

    /
    avio_skip(s->pb, 0x30);
    root_size = avio_rl32(s->pb);
    if (root_size > sizeof(root)) {
        av_log(s, AV_LOG_ERROR, "root directory size exceeds sector size\n");
        return AVERROR_INVALIDDATA;
    }
    avio_skip(s->pb, 4);
    root_sector = avio_rl32(s->pb);

    ret = seek_by_sector(s->pb, root_sector, 0);
    if (ret < 0)
        return ret;
    root_size = avio_read(s->pb, root, root_size);
    if (root_size < 0)
        return AVERROR_INVALIDDATA;

    /
    wtv->pb = wtvfile_open(s, root, root_size, ff_timeline_le16);
    if (!wtv->pb) {
        av_log(s, AV_LOG_ERROR, "timeline data missing\n");
        return AVERROR_INVALIDDATA;
    }

    ret = parse_chunks(s, SEEK_TO_DATA, 0, 0);
    if (ret < 0)
        return ret;
    avio_seek(wtv->pb, -32, SEEK_CUR);

    timeline_pos = avio_tell(s->pb); // save before opening another file

    /
    pb = wtvfile_open(s, root, root_size, ff_table_0_entries_legacy_attrib_le16);
    if (pb) {
        parse_legacy_attrib(s, pb);
        wtvfile_close(pb);
    }

    /
    if (s->nb_streams) {
        AVStream *st = s->streams[0];
        pb = wtvfile_open(s, root, root_size, ff_table_0_entries_time_le16);
        if (pb) {
            while(1) {
                uint64_t timestamp = avio_rl64(pb);
                uint64_t frame_nb  = avio_rl64(pb);
                if (avio_feof(pb))
                    break;
                ff_add_index_entry(&wtv->index_entries, &wtv->nb_index_entries, &wtv->index_entries_allocated_size,
                                   0, timestamp, frame_nb, 0, AVINDEX_KEYFRAME);
            }
            wtvfile_close(pb);

            if (wtv->nb_index_entries) {
                pb = wtvfile_open(s, root, root_size, ff_timeline_table_0_entries_Events_le16);
                if (pb) {
                    int i;
                    while (1) {
                        uint64_t frame_nb = avio_rl64(pb);
                        uint64_t position = avio_rl64(pb);
                        if (avio_feof(pb))
                            break;
                        for (i = wtv->nb_index_entries - 1; i >= 0; i--) {
                            AVIndexEntry *e = wtv->index_entries + i;
                            if (frame_nb > e->size)
                                break;
                            if (position > e->pos)
                                e->pos = position;
                        }
                    }
                    wtvfile_close(pb);
                    st->duration = wtv->index_entries[wtv->nb_index_entries - 1].timestamp;
                }
            }
        }
    }

    avio_seek(s->pb, timeline_pos, SEEK_SET);
    return 0;
}
