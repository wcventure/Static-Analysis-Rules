typedef struct  {
    int64_t frames_hdr_strm;
    int audio_strm_length;
    int packet_count;
    int entry;

    AVIIndex indexes;
} AVIStream ;
