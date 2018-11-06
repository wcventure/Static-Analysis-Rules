static int sync(AVFormatContext *s, int64_t *timestamp, int *flags, int *stream_index, int64_t *pos){
    RMDemuxContext *rm = s->priv_data;
    ByteIOContext *pb = s->pb;
    int len, num, res, i;
    AVStream *st;
    uint32_t state=0xFFFFFFFF;

    while(!url_feof(pb)){
        *pos= url_ftell(pb) - 3;
        if(rm->remaining_len > 0){
            num= rm->current_stream;
            len= rm->remaining_len;
            *timestamp = AV_NOPTS_VALUE;
            *flags= 0;
        }else{
            state= (state<<8) + get_byte(pb);

            if(state == MKBETAG('I', 'N', 'D', 'X')){
                len = get_be16(pb) - 6;
                if(len<0)
                    continue;
                goto skip;
            }

            if(state > (unsigned)0xFFFF || state < 12)
                continue;
            len=state;
            state= 0xFFFFFFFF;

            num = get_be16(pb);
            *timestamp = get_be32(pb);
            res= get_byte(pb); /
            *flags = get_byte(pb); /


            len -= 12;
        }
        for(i=0;i<s->nb_streams;i++) {
            st = s->streams[i];
            if (num == st->id)
                break;
        }
        if (i == s->nb_streams) {
skip:
            /
            url_fskip(pb, len);
            rm->remaining_len -= len;
            continue;
        }
        *stream_index= i;

        return len;
    }
    return -1;
}
