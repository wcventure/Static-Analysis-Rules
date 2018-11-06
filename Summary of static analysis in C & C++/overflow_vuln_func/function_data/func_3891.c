typedef struct FrameRateContext {
    const AVClass *class;
    // parameters
    AVRational dest_frame_rate;         ///< output frames per second
    int flags;                          ///< flags affecting frame rate conversion algorithm
    double scene_score;                 ///< score that denotes a scene change has happened
    int interp_start;                   ///< start of range to apply linear interpolation (same bitdepth as input)
    int interp_end;                     ///< end of range to apply linear interpolation (same bitdepth as input)
    int interp_start_param;             ///< start of range to apply linear interpolation
    int interp_end_param;               ///< end of range to apply linear interpolation

    int line_size[4];                   ///< bytes of pixel data per line for each plane
    int vsub;

    AVRational srce_time_base;          ///< timebase of source
    AVRational dest_time_base;          ///< timebase of destination

    av_pixelutils_sad_fn sad;           ///< Sum of the absolute difference function (scene detect only)
    double prev_mafd;                   ///< previous MAFD                           (scene detect only)

    int max;
    int bitdepth;
    AVFrame *work;

    AVFrame *f0;                        ///< last frame
    AVFrame *f1;                        ///< current frame
    int64_t pts0;                       ///< last frame pts in dest_time_base
    int64_t pts1;                       ///< current frame pts in dest_time_base
    int64_t delta;                      ///< pts1 to pts0 delta
    double score;                       ///< scene change score (f0 to f1)
    int flush;                          ///< 1 if the filter is being flushed
    int64_t start_pts;                  ///< pts of the first output frame
    int64_t n;                          ///< output frame counter

    blend_func blend;
} FrameRateContext;
