static int read_header(AVFormatContext *s)
{
    AVIOContext *pb = s->pb;
    CaffContext *caf  = s->priv_data;
    AVStream *st;
    uint32_t tag = 0;
    int found_data, ret;
    int64_t size, pos;

    avio_skip(pb, 8); /

    /
    if (avio_rb32(pb) != MKBETAG('d','e','s','c')) {
        av_log(s, AV_LOG_ERROR, "desc chunk not present\n");
        return AVERROR_INVALIDDATA;
    }
    size = avio_rb64(pb);
    if (size != 32)
        return AVERROR_INVALIDDATA;

    ret = read_desc_chunk(s);
    if (ret)
        return ret;
    st = s->streams[0];

    /
    found_data = 0;
    while (!url_feof(pb)) {

        /
        if (found_data && (caf->data_size < 0 || !pb->seekable))
            break;

        tag  = avio_rb32(pb);
        size = avio_rb64(pb);
        pos  = avio_tell(pb);
        if (url_feof(pb))
            break;

        switch (tag) {
        case MKBETAG('d','a','t','a'):
            avio_skip(pb, 4); /
            caf->data_start = avio_tell(pb);
            caf->data_size  = size < 0 ? -1 : size - 4;
            if (caf->data_size > 0 && pb->seekable)
                avio_skip(pb, caf->data_size);
            found_data = 1;
            break;

        case MKBETAG('c','h','a','n'):
            if ((ret = ff_mov_read_chan(s, s->pb, st, size)) < 0)
                return ret;
            break;

        /
        case MKBETAG('k','u','k','i'):
            if (read_kuki_chunk(s, size))
                return AVERROR_INVALIDDATA;
            break;

        /
        case MKBETAG('p','a','k','t'):
            if (read_pakt_chunk(s, size))
                return AVERROR_INVALIDDATA;
            break;

        case MKBETAG('i','n','f','o'):
            read_info_chunk(s, size);
            break;

        default:
#define _(x) ((x) >= ' ' ? (x) : ' ')
            av_log(s, AV_LOG_WARNING, "skipping CAF chunk: %08X (%c%c%c%c), size %"PRId64"\n",
                tag, _(tag>>24), _((tag>>16)&0xFF), _((tag>>8)&0xFF), _(tag&0xFF), size);
#undef _
        case MKBETAG('f','r','e','e'):
            if (size < 0)
                return AVERROR_INVALIDDATA;
            break;
        }

        if (size > 0) {
            if (pos + size < pos)
                return AVERROR_INVALIDDATA;
            avio_skip(pb, FFMAX(0, pos + size - avio_tell(pb)));
        }
    }

    if (!found_data)
        return AVERROR_INVALIDDATA;

    if (caf->bytes_per_packet > 0 && caf->frames_per_packet > 0) {
        if (caf->data_size > 0)
            st->nb_frames = (caf->data_size / caf->bytes_per_packet) * caf->frames_per_packet;
    } else if (st->nb_index_entries && st->duration > 0) {
        st->codec->bit_rate = st->codec->sample_rate * caf->data_size * 8 /
                              st->duration;
    } else {
        av_log(s, AV_LOG_ERROR, "Missing packet table. It is required when "
                                "block size or frame size are variable.\n");
        return AVERROR_INVALIDDATA;
    }

    avpriv_set_pts_info(st, 64, 1, st->codec->sample_rate);
    st->start_time = 0;

    /
    if (caf->data_size >= 0)
        avio_seek(pb, caf->data_start, SEEK_SET);

    return 0;
}
