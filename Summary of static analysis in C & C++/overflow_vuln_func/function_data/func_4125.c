static int decode_info_header(NUTContext *nut){
    AVFormatContext *s= nut->avf;
    ByteIOContext *bc = s->pb;
    uint64_t tmp;
    unsigned int stream_id_plus1, chapter_start, chapter_len, count;
    int chapter_id, i;
    int64_t value, end;
    char name[256], str_value[1024], type_str[256];
    const char *type;
    AVChapter *chapter= NULL;
    AVStream *st= NULL;

    end= get_packetheader(nut, bc, 1, INFO_STARTCODE);
    end += url_ftell(bc);

    GET_V(stream_id_plus1, tmp <= s->nb_streams)
    chapter_id   = get_s(bc);
    chapter_start= ff_get_v(bc);
    chapter_len  = ff_get_v(bc);
    count        = ff_get_v(bc);

    if(chapter_id && !stream_id_plus1){
        int64_t start= chapter_start / nut->time_base_count;
        chapter= ff_new_chapter(s, chapter_id,
                                nut->time_base[chapter_start % nut->time_base_count],
                                start, start + chapter_len, NULL);
    } else if(stream_id_plus1)
        st= s->streams[stream_id_plus1 - 1];

    for(i=0; i<count; i++){
        get_str(bc, name, sizeof(name));
        value= get_s(bc);
        if(value == -1){
            type= "UTF-8";
            get_str(bc, str_value, sizeof(str_value));
        }else if(value == -2){
            get_str(bc, type_str, sizeof(type_str));
            type= type_str;
            get_str(bc, str_value, sizeof(str_value));
        }else if(value == -3){
            type= "s";
            value= get_s(bc);
        }else if(value == -4){
            type= "t";
            value= ff_get_v(bc);
        }else if(value < -4){
            type= "r";
            get_s(bc);
        }else{
            type= "v";
        }

        if (stream_id_plus1 > s->nb_streams) {
            av_log(s, AV_LOG_ERROR, "invalid stream id for info packet\n");
            continue;
        }

        if(!strcmp(type, "UTF-8")){
            AVMetadata **metadata = NULL;
            if(chapter_id==0 && !strcmp(name, "Disposition"))
                set_disposition_bits(s, str_value, stream_id_plus1 - 1);
            else if(chapter)          metadata= &chapter->metadata;
            else if(stream_id_plus1)  metadata= &st->metadata;
            else                      metadata= &s->metadata;
            if(metadata && strcasecmp(name,"Uses")
               && strcasecmp(name,"Depends") && strcasecmp(name,"Replaces"))
                av_metadata_set(metadata, name, str_value);
        }
    }

    if(skip_reserved(bc, end) || get_checksum(bc)){
        av_log(s, AV_LOG_ERROR, "info header checksum mismatch\n");
        return -1;
    }
    return 0;
}
