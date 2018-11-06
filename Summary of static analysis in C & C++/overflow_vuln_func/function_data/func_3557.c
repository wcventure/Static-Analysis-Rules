int av_packet_split_side_data(AVPacket *pkt){
    if (!pkt->side_data_elems && pkt->size >12 && AV_RB64(pkt->data + pkt->size - 8) == FF_MERGE_MARKER){
        int i;
        unsigned int size;
        uint8_t *p;

        p = pkt->data + pkt->size - 8 - 5;
        for (i=1; ; i++){
            size = AV_RB32(p);
            if (size>INT_MAX || p - pkt->data < size)
                return 0;
            if (p[4]&128)
                break;
            p-= size+5;
        }

        pkt->side_data = av_malloc_array(i, sizeof(*pkt->side_data));
        if (!pkt->side_data)
            return AVERROR(ENOMEM);

        p= pkt->data + pkt->size - 8 - 5;
        for (i=0; ; i++){
            size= AV_RB32(p);
            av_assert0(size<=INT_MAX && p - pkt->data >= size);
            pkt->side_data[i].data = av_mallocz(size + AV_INPUT_BUFFER_PADDING_SIZE);
            pkt->side_data[i].size = size;
            pkt->side_data[i].type = p[4]&127;
            if (!pkt->side_data[i].data)
                return AVERROR(ENOMEM);
            memcpy(pkt->side_data[i].data, p-size, size);
            pkt->size -= size + 5;
            if(p[4]&128)
                break;
            p-= size+5;
        }
        pkt->size -= 8;
        pkt->side_data_elems = i+1;
        return 1;
    }
    return 0;
}
