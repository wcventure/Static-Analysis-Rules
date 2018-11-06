static int mov_write_tkhd_tag(AVIOContext *pb, MOVMuxContext *mov,
                              MOVTrack *track, AVStream *st)
{
    int64_t duration = av_rescale_rnd(track->track_duration, MOV_TIMESCALE,
                                      track->timescale, AV_ROUND_UP);
    int version = duration < INT32_MAX ? 0 : 1;
    int flags   = MOV_TKHD_FLAG_IN_MOVIE;
    int rotation = 0;
    int group   = 0;

    uint32_t *display_matrix = NULL;
    int      display_matrix_size, i;

    if (st) {
        if (mov->per_stream_grouping)
            group = st->index;
        else
            group = st->codecpar->codec_type;

        display_matrix = (uint32_t*)av_stream_get_side_data(st, AV_PKT_DATA_DISPLAYMATRIX,
                                                            &display_matrix_size);
        if (display_matrix && display_matrix_size < 9 * sizeof(*display_matrix))
            display_matrix = NULL;
    }

    if (track->flags & MOV_TRACK_ENABLED)
        flags |= MOV_TKHD_FLAG_ENABLED;

    if (track->mode == MODE_ISM)
        version = 1;

    (version == 1) ? avio_wb32(pb, 104) : avio_wb32(pb, 92); /
    ffio_wfourcc(pb, "tkhd");
    avio_w8(pb, version);
    avio_wb24(pb, flags);
    if (version == 1) {
        avio_wb64(pb, track->time);
        avio_wb64(pb, track->time);
    } else {
        avio_wb32(pb, track->time); /
        avio_wb32(pb, track->time); /
    }
    avio_wb32(pb, track->track_id); /
    avio_wb32(pb, 0); /
    if (!track->entry && mov->mode == MODE_ISM)
        (version == 1) ? avio_wb64(pb, UINT64_C(0xffffffffffffffff)) : avio_wb32(pb, 0xffffffff);
    else if (!track->entry)
        (version == 1) ? avio_wb64(pb, 0) : avio_wb32(pb, 0);
    else
        (version == 1) ? avio_wb64(pb, duration) : avio_wb32(pb, duration);

    avio_wb32(pb, 0); /
    avio_wb32(pb, 0); /
    avio_wb16(pb, 0); /
    avio_wb16(pb, group); /
    /
    if (track->par->codec_type == AVMEDIA_TYPE_AUDIO)
        avio_wb16(pb, 0x0100);
    else
        avio_wb16(pb, 0);
    avio_wb16(pb, 0); /

    /
    if (st && st->metadata) {
        AVDictionaryEntry *rot = av_dict_get(st->metadata, "rotate", NULL, 0);
        rotation = (rot && rot->value) ? atoi(rot->value) : 0;
    }
    if (display_matrix) {
        for (i = 0; i < 9; i++)
            avio_wb32(pb, display_matrix[i]);
    } else if (rotation == 90) {
        write_matrix(pb,  0,  1, -1,  0, track->par->height, 0);
    } else if (rotation == 180) {
        write_matrix(pb, -1,  0,  0, -1, track->par->width, track->par->height);
    } else if (rotation == 270) {
        write_matrix(pb,  0, -1,  1,  0, 0, track->par->width);
    } else {
        write_matrix(pb,  1,  0,  0,  1, 0, 0);
    }
    /
    if (st && (track->par->codec_type == AVMEDIA_TYPE_VIDEO ||
               track->par->codec_type == AVMEDIA_TYPE_SUBTITLE)) {
        if (track->mode == MODE_MOV) {
            avio_wb32(pb, track->par->width << 16);
            avio_wb32(pb, track->height << 16);
        } else {
            int64_t track_width_1616 = av_rescale(st->sample_aspect_ratio.num,
                                                  track->par->width * 0x10000LL,
                                                  st->sample_aspect_ratio.den);
            if (!track_width_1616 ||
                track->height != track->par->height ||
                track_width_1616 > UINT32_MAX)
                track_width_1616 = track->par->width * 0x10000ULL;
            if (track_width_1616 > UINT32_MAX) {
                av_log(mov->fc, AV_LOG_WARNING, "track width too large\n");
                track_width_1616 = 0;
            }
            avio_wb32(pb, track_width_1616);
            avio_wb32(pb, track->height * 0x10000U);
        }
    } else {
        avio_wb32(pb, 0);
        avio_wb32(pb, 0);
    }
    return 0x5c;
}
