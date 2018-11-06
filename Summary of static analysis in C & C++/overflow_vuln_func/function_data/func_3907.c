static int asf_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    ASFContext *asf = s->priv_data;
    AVIOContext *pb = s->pb;
    int ret, i;

    if ((avio_tell(pb) >= asf->data_offset + asf->data_size) &&
        !(asf->b_flags & ASF_FLAG_BROADCAST))
        return AVERROR_EOF;
    while (!pb->eof_reached) {
        if (asf->state == PARSE_PACKET_HEADER) {
            asf_read_packet_header(s);
            if (pb->eof_reached)
                break;
            if (!asf->nb_mult_left)
                asf->state = READ_SINGLE;
            else
                asf->state = READ_MULTI;
        }
        ret = asf_read_payload(s, pkt);
        if (ret == AVERROR(EAGAIN)) {
            asf->state = PARSE_PACKET_HEADER;
            continue;
        }
        else if (ret < 0)
            return ret;

        switch (asf->state) {
        case READ_SINGLE:
            if (!asf->sub_left)
                asf->state = PARSE_PACKET_HEADER;
            break;
        case READ_MULTI_SUB:
            if (!asf->sub_left && !asf->nb_mult_left) {
                asf->state = PARSE_PACKET_HEADER;
                if (!asf->return_subpayload &&
                    (avio_tell(pb) <= asf->packet_offset +
                     asf->packet_size - asf->pad_len))
                    avio_skip(pb, asf->pad_len); // skip padding
                if (asf->packet_offset + asf->packet_size > avio_tell(pb))
                    avio_seek(pb, asf->packet_offset + asf->packet_size, SEEK_SET);
            } else if (!asf->sub_left)
                asf->state = READ_MULTI;
            break;
        case READ_MULTI:
            if (!asf->nb_mult_left) {
                asf->state = PARSE_PACKET_HEADER;
                if (!asf->return_subpayload &&
                    (avio_tell(pb) <= asf->packet_offset +
                     asf->packet_size - asf->pad_len))
                    avio_skip(pb, asf->pad_len); // skip padding
                if (asf->packet_offset + asf->packet_size > avio_tell(pb))
                    avio_seek(pb, asf->packet_offset + asf->packet_size, SEEK_SET);
            }
            break;
        }
        if (asf->return_subpayload) {
            asf->return_subpayload = 0;
            return 0;
        }
        for (i = 0; i < s->nb_streams; i++) {
            ASFPacket *asf_pkt = &asf->asf_st[i]->pkt;
            if (asf_pkt && !asf_pkt->size_left && asf_pkt->data_size) {
                if (asf->asf_st[i]->span > 1 &&
                    asf->asf_st[i]->type == AVMEDIA_TYPE_AUDIO)
                    if ((ret = asf_deinterleave(s, asf_pkt, i)) < 0)
                        return ret;
                av_packet_move_ref(pkt, &asf_pkt->avpkt);
                pkt->stream_index  = asf->asf_st[i]->index;
                pkt->flags         = asf_pkt->flags;
                pkt->dts           = asf_pkt->dts - asf->preroll;
                asf_pkt->data_size = 0;
                asf_pkt->frame_num = 0;
                return 0;
            }
        }
    }

    if (pb->eof_reached)
        return AVERROR_EOF;

    return 0;
}
