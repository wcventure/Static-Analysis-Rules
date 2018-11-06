typedef struct ThpDemuxContext {
    int              version;
    int              first_frame;
    int              first_framesz;
    int              last_frame;
    int              compoff;
    int              framecnt;
    AVRational       fps;
    int              frame;
    int              next_frame;
    int              next_framesz;
    int              video_stream_index;
    int              audio_stream_index;
    int              compcount;
    unsigned char    components[16];
    AVStream*        vst;
    int              has_audio;
    unsigned         audiosize;
} ThpDemuxContext;
