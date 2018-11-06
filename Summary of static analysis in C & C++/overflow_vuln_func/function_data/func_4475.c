typedef struct DASHContext {
    const AVClass *class;  /
    int window_size;
    int extra_window_size;
    int min_seg_duration;
    int remove_at_exit;
    int use_template;
    int use_timeline;
    int single_file;
    OutputStream *streams;
    int has_video, has_audio;
    int last_duration;
    int total_duration;
    char availability_start_time[100];
    char dirname[1024];
    const char *single_file_name;
    const char *init_seg_name;
    const char *media_seg_name;
} DASHContext;
