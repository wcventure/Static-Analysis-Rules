typedef struct HLSContext {
    const AVClass *class;  // Class for private options.
    int number;
    int64_t sequence;
    AVOutputFormat *oformat;
    AVFormatContext *avf;
    float time;            // Set by a private option.
    int  size;             // Set by a private option.
    int  wrap;             // Set by a private option.
    int64_t recording_time;
    int has_video;
    int64_t start_pts;
    int64_t end_pts;
    int nb_entries;
    ListEntry *list;
    ListEntry *end_list;
    char *basename;
    AVIOContext *pb;
} HLSContext;
