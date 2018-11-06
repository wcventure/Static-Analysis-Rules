typedef struct OptionsContext {
    /
    int64_t start_time;
    const char *format;

    SpecifierOpt *codec_names;
    int        nb_codec_names;
    SpecifierOpt *audio_channels;
    int        nb_audio_channels;
    SpecifierOpt *audio_sample_rate;
    int        nb_audio_sample_rate;
    SpecifierOpt *rematrix_volume;
    int        nb_rematrix_volume;
    SpecifierOpt *frame_rates;
    int        nb_frame_rates;
    SpecifierOpt *frame_sizes;
    int        nb_frame_sizes;
    SpecifierOpt *frame_pix_fmts;
    int        nb_frame_pix_fmts;

    /
    int64_t input_ts_offset;
    int rate_emu;

    SpecifierOpt *ts_scale;
    int        nb_ts_scale;
    SpecifierOpt *dump_attachment;
    int        nb_dump_attachment;

    /
    StreamMap *stream_maps;
    int     nb_stream_maps;
    AudioChannelMap *audio_channel_maps; ///< one info entry per -map_channel
    int           nb_audio_channel_maps; ///< number of (valid) -map_channel settings
    int metadata_global_manual;
    int metadata_streams_manual;
    int metadata_chapters_manual;
    const char **attachments;
    int       nb_attachments;

    int chapters_input_file;

    int64_t recording_time;
    uint64_t limit_filesize;
    float mux_preload;
    float mux_max_delay;

    int video_disable;
    int audio_disable;
    int subtitle_disable;
    int data_disable;

    /
    int   *streamid_map;
    int nb_streamid_map;

    SpecifierOpt *metadata;
    int        nb_metadata;
    SpecifierOpt *max_frames;
    int        nb_max_frames;
    SpecifierOpt *bitstream_filters;
    int        nb_bitstream_filters;
    SpecifierOpt *codec_tags;
    int        nb_codec_tags;
    SpecifierOpt *sample_fmts;
    int        nb_sample_fmts;
    SpecifierOpt *qscale;
    int        nb_qscale;
    SpecifierOpt *forced_key_frames;
    int        nb_forced_key_frames;
    SpecifierOpt *force_fps;
    int        nb_force_fps;
    SpecifierOpt *frame_aspect_ratios;
    int        nb_frame_aspect_ratios;
    SpecifierOpt *rc_overrides;
    int        nb_rc_overrides;
    SpecifierOpt *intra_matrices;
    int        nb_intra_matrices;
    SpecifierOpt *inter_matrices;
    int        nb_inter_matrices;
    SpecifierOpt *top_field_first;
    int        nb_top_field_first;
    SpecifierOpt *metadata_map;
    int        nb_metadata_map;
    SpecifierOpt *presets;
    int        nb_presets;
    SpecifierOpt *copy_initial_nonkeyframes;
    int        nb_copy_initial_nonkeyframes;
    SpecifierOpt *filters;
    int        nb_filters;
} OptionsContext;
