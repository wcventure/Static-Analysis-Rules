static int asf_read_frame_header(AVFormatContext *s, AVIOContext *pb){
    ASFContext *asf = s->priv_data;
    ASFStream *asfst;
    int rsize = 1;
    int num = avio_r8(pb);
    int i;
    int64_t ts0, ts1 av_unused;

    asf->packet_segments--;
    asf->packet_key_frame = num >> 7;
    asf->stream_index = asf->asfid2avid[num & 0x7f];
    asfst = &asf->streams[num & 0x7f];
    // sequence should be ignored!
    DO_2BITS(asf->packet_property >> 4, asf->packet_seq, 0);
    DO_2BITS(asf->packet_property >> 2, asf->packet_frag_offset, 0);
    DO_2BITS(asf->packet_property, asf->packet_replic_size, 0);
    av_dlog(asf, "key:%d stream:%d seq:%d offset:%d replic_size:%d\n",
            asf->packet_key_frame, asf->stream_index, asf->packet_seq,
            asf->packet_frag_offset, asf->packet_replic_size);
    if (rsize+asf->packet_replic_size > asf->packet_size_left) {
        av_log(s, AV_LOG_ERROR, "packet_replic_size %d is invalid\n", asf->packet_replic_size);
        return AVERROR_INVALIDDATA;
    }
    if (asf->packet_replic_size >= 8) {
        int64_t end = avio_tell(pb) + asf->packet_replic_size;
        AVRational aspect;
        asf->packet_obj_size = avio_rl32(pb);
        if(asf->packet_obj_size >= (1<<24) || asf->packet_obj_size <= 0){
            av_log(s, AV_LOG_ERROR, "packet_obj_size invalid\n");
            return AVERROR_INVALIDDATA;
        }
        asf->packet_frag_timestamp = avio_rl32(pb); // timestamp

        for (i=0; i<asfst->payload_ext_ct; i++) {
            ASFPayload *p = &asfst->payload[i];
            int size = p->size;
            int64_t payend;
            if(size == 0xFFFF)
                size = avio_rl16(pb);
            payend = avio_tell(pb) + size;
            if (payend > end) {
                av_log(s, AV_LOG_ERROR, "too long payload\n");
                break;
            }
            switch(p->type) {
            case 0x50:
//              duration = avio_rl16(pb);
                break;
            case 0x54:
                aspect.num = avio_r8(pb);
                aspect.den = avio_r8(pb);
                if (aspect.num > 0 && aspect.den > 0 && asf->stream_index >= 0) {
                    s->streams[asf->stream_index]->sample_aspect_ratio = aspect;
                }
                break;
            case 0x2A:
                avio_skip(pb, 8);
                ts0= avio_rl64(pb);
                ts1= avio_rl64(pb);
                if(ts0!= -1) asf->packet_frag_timestamp= ts0/10000;
                else         asf->packet_frag_timestamp= AV_NOPTS_VALUE;
                break;
            case 0x5B:
            case 0xB7:
            case 0xCC:
            case 0xC0:
            case 0xA0:
                //unknown
                break;
            }
            avio_seek(pb, payend, SEEK_SET);
        }

        avio_seek(pb, end, SEEK_SET);
        rsize += asf->packet_replic_size; // FIXME - check validity
    } else if (asf->packet_replic_size==1){
        // multipacket - frag_offset is beginning timestamp
        asf->packet_time_start = asf->packet_frag_offset;
        asf->packet_frag_offset = 0;
        asf->packet_frag_timestamp = asf->packet_timestamp;

        asf->packet_time_delta = avio_r8(pb);
        rsize++;
    }else if(asf->packet_replic_size!=0){
        av_log(s, AV_LOG_ERROR, "unexpected packet_replic_size of %d\n", asf->packet_replic_size);
        return AVERROR_INVALIDDATA;
    }
    if (asf->packet_flags & 0x01) {
        DO_2BITS(asf->packet_segsizetype >> 6, asf->packet_frag_size, 0); // 0 is illegal
        if (rsize > asf->packet_size_left) {
            av_log(s, AV_LOG_ERROR, "packet_replic_size is invalid\n");
            return AVERROR_INVALIDDATA;
        } else if(asf->packet_frag_size > asf->packet_size_left - rsize){
            if (asf->packet_frag_size > asf->packet_size_left - rsize + asf->packet_padsize) {
                av_log(s, AV_LOG_ERROR, "packet_frag_size is invalid (%d-%d)\n", asf->packet_size_left, rsize);
                return AVERROR_INVALIDDATA;
            } else {
                int diff = asf->packet_frag_size - (asf->packet_size_left - rsize);
                asf->packet_size_left += diff;
                asf->packet_padsize   -= diff;
            }
        }
    } else {
        asf->packet_frag_size = asf->packet_size_left - rsize;
    }
    if (asf->packet_replic_size == 1) {
        asf->packet_multi_size = asf->packet_frag_size;
        if (asf->packet_multi_size > asf->packet_size_left)
            return AVERROR_INVALIDDATA;
    }
    asf->packet_size_left -= rsize;

    return 0;
}
