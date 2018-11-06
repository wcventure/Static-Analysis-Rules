static int matroska_parse_block(MatroskaDemuxContext *matroska, uint8_t *data,
                                int size, int64_t pos, uint64_t cluster_time,
                                uint64_t duration, int is_keyframe,
                                int64_t cluster_pos)
{
    uint64_t timecode = AV_NOPTS_VALUE;
    MatroskaTrack *track;
    int res = 0;
    AVStream *st;
    AVPacket *pkt;
    int16_t block_time;
    uint32_t *lace_size = NULL;
    int n, flags, laces = 0;
    uint64_t num;

    if ((n = matroska_ebmlnum_uint(matroska, data, size, &num)) < 0) {
        av_log(matroska->ctx, AV_LOG_ERROR, "EBML block data error\n");
        return res;
    }
    data += n;
    size -= n;

    track = matroska_find_track_by_num(matroska, num);
    if (size <= 3 || !track || !track->stream) {
        av_log(matroska->ctx, AV_LOG_INFO,
               "Invalid stream %"PRIu64" or size %u\n", num, size);
        return AVERROR_INVALIDDATA;
    }
    st = track->stream;
    if (st->discard >= AVDISCARD_ALL)
        return res;
    if (duration == AV_NOPTS_VALUE)
        duration = track->default_duration / matroska->time_scale;

    block_time = AV_RB16(data);
    data += 2;
    flags = *data++;
    size -= 3;
    if (is_keyframe == -1)
        is_keyframe = flags & 0x80 ? AV_PKT_FLAG_KEY : 0;

    if (cluster_time != (uint64_t)-1
        && (block_time >= 0 || cluster_time >= -block_time)) {
        timecode = cluster_time + block_time;
        if (track->type == MATROSKA_TRACK_TYPE_SUBTITLE
            && timecode < track->end_timecode)
            is_keyframe = 0;  /
        if (is_keyframe)
            av_add_index_entry(st, cluster_pos, timecode, 0,0,AVINDEX_KEYFRAME);
        track->end_timecode = FFMAX(track->end_timecode, timecode+duration);
    }

    if (matroska->skip_to_keyframe && track->type != MATROSKA_TRACK_TYPE_SUBTITLE) {
        if (!is_keyframe || timecode < matroska->skip_to_timecode)
            return res;
        matroska->skip_to_keyframe = 0;
    }

    switch ((flags & 0x06) >> 1) {
        case 0x0: /
            laces = 1;
            lace_size = av_mallocz(sizeof(int));
            lace_size[0] = size;
            break;

        case 0x1: /
        case 0x2: /
        case 0x3: /
            assert(size>0); // size <=3 is checked before size-=3 above
            laces = (*data) + 1;
            data += 1;
            size -= 1;
            lace_size = av_mallocz(laces * sizeof(int));

            switch ((flags & 0x06) >> 1) {
                case 0x1: / {
                    uint8_t temp;
                    uint32_t total = 0;
                    for (n = 0; res == 0 && n < laces - 1; n++) {
                        while (1) {
                            if (size == 0) {
                                res = -1;
                                break;
                            }
                            temp = *data;
                            lace_size[n] += temp;
                            data += 1;
                            size -= 1;
                            if (temp != 0xff)
                                break;
                        }
                        total += lace_size[n];
                    }
                    lace_size[n] = size - total;
                    break;
                }

                case 0x2: /
                    for (n = 0; n < laces; n++)
                        lace_size[n] = size / laces;
                    break;

                case 0x3: / {
                    uint32_t total;
                    n = matroska_ebmlnum_uint(matroska, data, size, &num);
                    if (n < 0) {
                        av_log(matroska->ctx, AV_LOG_INFO,
                               "EBML block data error\n");
                        break;
                    }
                    data += n;
                    size -= n;
                    total = lace_size[0] = num;
                    for (n = 1; res == 0 && n < laces - 1; n++) {
                        int64_t snum;
                        int r;
                        r = matroska_ebmlnum_sint(matroska, data, size, &snum);
                        if (r < 0) {
                            av_log(matroska->ctx, AV_LOG_INFO,
                                   "EBML block data error\n");
                            break;
                        }
                        data += r;
                        size -= r;
                        lace_size[n] = lace_size[n - 1] + snum;
                        total += lace_size[n];
                    }
                    lace_size[n] = size - total;
                    break;
                }
            }
            break;
    }

    if (res == 0) {
        for (n = 0; n < laces; n++) {
            if ((st->codec->codec_id == CODEC_ID_RA_288 ||
                 st->codec->codec_id == CODEC_ID_COOK ||
                 st->codec->codec_id == CODEC_ID_SIPR ||
                 st->codec->codec_id == CODEC_ID_ATRAC3) &&
                 st->codec->block_align && track->audio.sub_packet_size) {
                int a = st->codec->block_align;
                int sps = track->audio.sub_packet_size;
                int cfs = track->audio.coded_framesize;
                int h = track->audio.sub_packet_h;
                int y = track->audio.sub_packet_cnt;
                int w = track->audio.frame_size;
                int x;

                if (!track->audio.pkt_cnt) {
                    if (track->audio.sub_packet_cnt == 0)
                        track->audio.buf_timecode = timecode;
                    if (st->codec->codec_id == CODEC_ID_RA_288)
                        for (x=0; x<h/2; x++)
                            memcpy(track->audio.buf+x*2*w+y*cfs,
                                   data+x*cfs, cfs);
                    else if (st->codec->codec_id == CODEC_ID_SIPR)
                        memcpy(track->audio.buf + y*w, data, w);
                    else
                        for (x=0; x<w/sps; x++)
                            memcpy(track->audio.buf+sps*(h*x+((h+1)/2)*(y&1)+(y>>1)), data+x*sps, sps);

                    if (++track->audio.sub_packet_cnt >= h) {
                        if (st->codec->codec_id == CODEC_ID_SIPR)
                            ff_rm_reorder_sipr_data(track->audio.buf, h, w);
                        track->audio.sub_packet_cnt = 0;
                        track->audio.pkt_cnt = h*w / a;
                    }
                }
                while (track->audio.pkt_cnt) {
                    pkt = av_mallocz(sizeof(AVPacket));
                    av_new_packet(pkt, a);
                    memcpy(pkt->data, track->audio.buf
                           + a * (h*w / a - track->audio.pkt_cnt--), a);
                    pkt->pts = track->audio.buf_timecode;
                    track->audio.buf_timecode = AV_NOPTS_VALUE;
                    pkt->pos = pos;
                    pkt->stream_index = st->index;
                    dynarray_add(&matroska->packets,&matroska->num_packets,pkt);
                }
            } else {
                MatroskaTrackEncoding *encodings = track->encodings.elem;
                int offset = 0, pkt_size = lace_size[n];
                uint8_t *pkt_data = data;

                if (pkt_size > size) {
                    av_log(matroska->ctx, AV_LOG_ERROR, "Invalid packet size\n");
                    break;
                }

                if (encodings && encodings->scope & 1) {
                    offset = matroska_decode_buffer(&pkt_data,&pkt_size, track);
                    if (offset < 0)
                        continue;
                }

                pkt = av_mallocz(sizeof(AVPacket));
                /
                if (av_new_packet(pkt, pkt_size+offset) < 0) {
                    av_free(pkt);
                    res = AVERROR(ENOMEM);
                    break;
                }
                if (offset)
                    memcpy (pkt->data, encodings->compression.settings.data, offset);
                memcpy (pkt->data+offset, pkt_data, pkt_size);

                if (pkt_data != data)
                    av_free(pkt_data);

                if (n == 0)
                    pkt->flags = is_keyframe;
                pkt->stream_index = st->index;

                if (track->ms_compat)
                    pkt->dts = timecode;
                else
                    pkt->pts = timecode;
                pkt->pos = pos;
                if (st->codec->codec_id == CODEC_ID_TEXT)
                    pkt->convergence_duration = duration;
                else if (track->type != MATROSKA_TRACK_TYPE_SUBTITLE)
                    pkt->duration = duration;

                if (st->codec->codec_id == CODEC_ID_SSA)
                    matroska_fix_ass_packet(matroska, pkt, duration);

                if (matroska->prev_pkt &&
                    timecode != AV_NOPTS_VALUE &&
                    matroska->prev_pkt->pts == timecode &&
                    matroska->prev_pkt->stream_index == st->index &&
                    st->codec->codec_id == CODEC_ID_SSA)
                    matroska_merge_packets(matroska->prev_pkt, pkt);
                else {
                    dynarray_add(&matroska->packets,&matroska->num_packets,pkt);
                    matroska->prev_pkt = pkt;
                }
            }

            if (timecode != AV_NOPTS_VALUE)
                timecode = duration ? timecode + duration : AV_NOPTS_VALUE;
            data += lace_size[n];
            size -= lace_size[n];
        }
    }

    av_free(lace_size);
    return res;
}
