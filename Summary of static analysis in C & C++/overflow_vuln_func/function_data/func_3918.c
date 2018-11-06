typedef struct PSNRContext {
    const AVClass *class;
    FFDualInputContext dinput;
    double mse, min_mse, max_mse, mse_comp[4];
    uint64_t nb_frames;
    FILE *stats_file;
    char *stats_file_str;
    int max[4], average_max;
    int is_rgb;
    uint8_t rgba_map[4];
    char comps[4];
    int nb_components;
    int planewidth[4];
    int planeheight[4];
    double planeweight[4];

    void (*compute_mse)(struct PSNRContext *s,
                        const uint8_t *m[4], const int ml[4],
                        const uint8_t *r[4], const int rl[4],
                        int w, int h, double mse[4]);
} PSNRContext;
