typedef struct OutputStream {
    int file_index;          /
    int index;               /
    int source_index;        /
    AVStream *st;            /
    int encoding_needed;     /
    int frame_number;
    /
    // double sync_ipts;        /
    struct InputStream *sync_ist; /
    int64_t sync_opts;       / // FIXME look at frame_number
    /
    int64_t first_pts;
    AVBitStreamFilterContext *bitstream_filters;
    AVCodec *enc;
    int64_t max_frames;
    AVFrame *output_frame;
    AVFrame *filtered_frame;

    /
    AVRational frame_rate;
    int force_fps;
    int top_field_first;

    float frame_aspect_ratio;
    float last_quality;

    /
    int64_t *forced_kf_pts;
    int forced_kf_count;
    int forced_kf_index;

    /
    int audio_resample;
    AVAudioResampleContext *avr;
    int resample_sample_fmt;
    int resample_channels;
    uint64_t resample_channel_layout;
    int resample_sample_rate;
    AVFifoBuffer *fifo;     /
    FILE *logfile;

    OutputFilter *filter;
    char *avfilter;

    int64_t sws_flags;
    AVDictionary *opts;
    int is_past_recording_time;
    int stream_copy;
    const char *attachment_filename;
    int copy_initial_nonkeyframes;

    enum PixelFormat pix_fmts[2];
} OutputStream;
