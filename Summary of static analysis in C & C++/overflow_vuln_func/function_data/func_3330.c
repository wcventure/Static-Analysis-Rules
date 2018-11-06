static int asf_read_frame_header(AVFormatContext *s, AVIOContext *pb){
    ASFContext *asf = s->priv_data;
    int rsize = 1;
    int num = avio_r8(pb);
    int64_t ts0, ts1;

    asf->packet_segments--;
    asf->packet_key_frame = num >> 7;
    asf->stream_index = asf->asfid2avid[num & 0x7f];
    // sequence should be ignored!
    DO_2BITS(asf->packet_property >> 4, asf->packet_seq, 0);
    DO_2BITS(asf->packet_property >> 2, asf->packet_frag_offset, 0);
    DO_2BITS(asf->packet_property, asf->packet_replic_size, 0);
//printf("key:%d stream:%d seq:%d offset:%d replic_size:%d\n", asf->packet_key_frame, asf->stream_index, asf->packet_seq, //asf->packet_frag_offset, asf->packet_replic_size);
    if (asf->packet_replic_size >= 8) {
        asf->packet_obj_size = avio_rl32(pb);
        if(asf->packet_obj_size >= (1<<24) || asf->packet_obj_size <= 0){
            av_log(s, AV_LOG_ERROR, "packet_obj_size invalid\n");
            return -1;
        }
        asf->packet_frag_timestamp = avio_rl32(pb); // timestamp
        if(asf->packet_replic_size >= 8+38+4){
//            for(i=0; i<asf->packet_replic_size-8; i++)
//                av_log(s, AV_LOG_DEBUG, "%02X ",avio_r8(pb));
//            av_log(s, AV_LOG_DEBUG, "\n");
            avio_skip(pb, 10);
            ts0= avio_rl64(pb);
            ts1= avio_rl64(pb);
            avio_skip(pb, 12);
            avio_rl32(pb);
            avio_skip(pb, asf->packet_replic_size - 8 - 38 - 4);
            if(ts0!= -1) asf->packet_frag_timestamp= ts0/10000;
            else         asf->packet_frag_timestamp= AV_NOPTS_VALUE;
        }else
            avio_skip(pb, asf->packet_replic_size - 8);
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
        return -1;
    }
    if (asf->packet_flags & 0x01) {
        DO_2BITS(asf->packet_segsizetype >> 6, asf->packet_frag_size, 0); // 0 is illegal
        if(asf->packet_frag_size > asf->packet_size_left - rsize){
            av_log(s, AV_LOG_ERROR, "packet_frag_size is invalid\n");
            return -1;
        }
        //printf("Fragsize %d\n", asf->packet_frag_size);
    } else {
        asf->packet_frag_size = asf->packet_size_left - rsize;
        //printf("Using rest  %d %d %d\n", asf->packet_frag_size, asf->packet_size_left, rsize);
    }
    if (asf->packet_replic_size == 1) {
        asf->packet_multi_size = asf->packet_frag_size;
        if (asf->packet_multi_size > asf->packet_size_left)
            return -1;
    }
    asf->packet_size_left -= rsize;
    //printf("___objsize____  %d   %d    rs:%d\n", asf->packet_obj_size, asf->packet_frag_offset, rsize);

    return 0;
}
