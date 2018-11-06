static int decode_main_header(NUTContext *nut)
{
    AVFormatContext *s = nut->avf;
    AVIOContext *bc    = s->pb;
    uint64_t tmp, end;
    unsigned int stream_count;
    int i, j, count;
    int tmp_stream, tmp_mul, tmp_pts, tmp_size, tmp_res, tmp_head_idx;

    end  = get_packetheader(nut, bc, 1, MAIN_STARTCODE);
    end += avio_tell(bc);

    nut->version = ffio_read_varlen(bc);
    if (nut->version < NUT_MIN_VERSION &&
        nut->version > NUT_MAX_VERSION) {
        av_log(s, AV_LOG_ERROR, "Version %d not supported.\n",
               nut->version);
        return AVERROR(ENOSYS);
    }
    if (nut->version > 3)
        nut->minor_version = ffio_read_varlen(bc);

    GET_V(stream_count, tmp > 0 && tmp <= NUT_MAX_STREAMS);

    nut->max_distance = ffio_read_varlen(bc);
    if (nut->max_distance > 65536) {
        av_log(s, AV_LOG_DEBUG, "max_distance %d\n", nut->max_distance);
        nut->max_distance = 65536;
    }

    GET_V(nut->time_base_count, tmp > 0 && tmp < INT_MAX / sizeof(AVRational));
    nut->time_base = av_malloc_array(nut->time_base_count, sizeof(AVRational));
    if (!nut->time_base)
        return AVERROR(ENOMEM);

    for (i = 0; i < nut->time_base_count; i++) {
        GET_V(nut->time_base[i].num, tmp > 0 && tmp < (1ULL << 31));
        GET_V(nut->time_base[i].den, tmp > 0 && tmp < (1ULL << 31));
        if (av_gcd(nut->time_base[i].num, nut->time_base[i].den) != 1) {
            av_log(s, AV_LOG_ERROR, "time base invalid\n");
            return AVERROR_INVALIDDATA;
        }
    }
    tmp_pts      = 0;
    tmp_mul      = 1;
    tmp_stream   = 0;
    tmp_head_idx = 0;
    for (i = 0; i < 256;) {
        int tmp_flags  = ffio_read_varlen(bc);
        int tmp_fields = ffio_read_varlen(bc);

        if (tmp_fields > 0)
            tmp_pts = get_s(bc);
        if (tmp_fields > 1)
            tmp_mul = ffio_read_varlen(bc);
        if (tmp_fields > 2)
            tmp_stream = ffio_read_varlen(bc);
        if (tmp_fields > 3)
            tmp_size = ffio_read_varlen(bc);
        else
            tmp_size = 0;
        if (tmp_fields > 4)
            tmp_res = ffio_read_varlen(bc);
        else
            tmp_res = 0;
        if (tmp_fields > 5)
            count = ffio_read_varlen(bc);
        else
            count = tmp_mul - tmp_size;
        if (tmp_fields > 6)
            get_s(bc);
        if (tmp_fields > 7)
            tmp_head_idx = ffio_read_varlen(bc);

        while (tmp_fields-- > 8)
            ffio_read_varlen(bc);

        if (count == 0 || i + count > 256) {
            av_log(s, AV_LOG_ERROR, "illegal count %d at %d\n", count, i);
            return AVERROR_INVALIDDATA;
        }
        if (tmp_stream >= stream_count) {
            av_log(s, AV_LOG_ERROR, "illegal stream number\n");
            return AVERROR_INVALIDDATA;
        }

        for (j = 0; j < count; j++, i++) {
            if (i == 'N') {
                nut->frame_code[i].flags = FLAG_INVALID;
                j--;
                continue;
            }
            nut->frame_code[i].flags          = tmp_flags;
            nut->frame_code[i].pts_delta      = tmp_pts;
            nut->frame_code[i].stream_id      = tmp_stream;
            nut->frame_code[i].size_mul       = tmp_mul;
            nut->frame_code[i].size_lsb       = tmp_size + j;
            nut->frame_code[i].reserved_count = tmp_res;
            nut->frame_code[i].header_idx     = tmp_head_idx;
        }
    }
    av_assert0(nut->frame_code['N'].flags == FLAG_INVALID);

    if (end > avio_tell(bc) + 4) {
        int rem = 1024;
        GET_V(nut->header_count, tmp < 128U);
        nut->header_count++;
        for (i = 1; i < nut->header_count; i++) {
            uint8_t *hdr;
            GET_V(nut->header_len[i], tmp > 0 && tmp < 256);
            rem -= nut->header_len[i];
            if (rem < 0) {
                av_log(s, AV_LOG_ERROR, "invalid elision header\n");
                return AVERROR_INVALIDDATA;
            }
            hdr = av_malloc(nut->header_len[i]);
            if (!hdr)
                return AVERROR(ENOMEM);
            avio_read(bc, hdr, nut->header_len[i]);
            nut->header[i] = hdr;
        }
        av_assert0(nut->header_len[0] == 0);
    }

    // flags had been effectively introduced in version 4
    if (nut->version > 3 && end > avio_tell(bc) + 4) {
        nut->flags = ffio_read_varlen(bc);
    }

    if (skip_reserved(bc, end) || ffio_get_checksum(bc)) {
        av_log(s, AV_LOG_ERROR, "main header checksum mismatch\n");
        return AVERROR_INVALIDDATA;
    }

    nut->stream = av_calloc(stream_count, sizeof(StreamContext));
    if (!nut->stream)
        return AVERROR(ENOMEM);
    for (i = 0; i < stream_count; i++)
        avformat_new_stream(s, NULL);

    return 0;
}
