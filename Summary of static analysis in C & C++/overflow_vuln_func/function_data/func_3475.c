typedef struct DiracContext {
    AVCodecContext *avctx;
    MpegvideoEncDSPContext mpvencdsp;
    VideoDSPContext vdsp;
    DiracDSPContext diracdsp;
    DiracVersionInfo version;
    GetBitContext gb;
    AVDiracSeqHeader seq;
    int seen_sequence_header;
    int frame_number;           /
    Plane plane[3];
    int chroma_x_shift;
    int chroma_y_shift;

    int bit_depth;              /
    int pshift;                 /

    int zero_res;               /
    int is_arith;               /
    int core_syntax;            /
    int low_delay;              /
    int hq_picture;             /
    int ld_picture;             /
    int dc_prediction;          /
    int globalmc_flag;          /
    int num_refs;               /

    /
    unsigned wavelet_depth;     /
    unsigned wavelet_idx;

    /
    unsigned old_delta_quant;
    unsigned codeblock_mode;

    unsigned num_x;              /
    unsigned num_y;              /

    struct {
        unsigned width;
        unsigned height;
    } codeblock[MAX_DWT_LEVELS+1];

    struct {
        AVRational bytes;       /
        uint8_t quant[MAX_DWT_LEVELS][4]; /
    } lowdelay;

    struct {
        unsigned prefix_bytes;
        unsigned size_scaler;
    } highquality;

    struct {
        int pan_tilt[2];        /
        int zrs[2][2];          /
        int perspective[2];     /
        unsigned zrs_exp;
        unsigned perspective_exp;
    } globalmc[2];

    /
    uint8_t mv_precision;       /
    int16_t weight[2];          /
    unsigned weight_log2denom;  /

    int blwidth;                /
    int blheight;               /
    int sbwidth;                /
    int sbheight;               /

    uint8_t *sbsplit;
    DiracBlock *blmotion;

    uint8_t *edge_emu_buffer[4];
    uint8_t *edge_emu_buffer_base;

    uint16_t *mctmp;            /
    uint8_t *mcscratch;
    int buffer_stride;

    DECLARE_ALIGNED(16, uint8_t, obmc_weight)[3][MAX_BLOCKSIZE*MAX_BLOCKSIZE];

    void (*put_pixels_tab[4])(uint8_t *dst, const uint8_t *src[5], int stride, int h);
    void (*avg_pixels_tab[4])(uint8_t *dst, const uint8_t *src[5], int stride, int h);
    void (*add_obmc)(uint16_t *dst, const uint8_t *src, int stride, const uint8_t *obmc_weight, int yblen);
    dirac_weight_func weight_func;
    dirac_biweight_func biweight_func;

    DiracFrame *current_picture;
    DiracFrame *ref_pics[2];

    DiracFrame *ref_frames[MAX_REFERENCE_FRAMES+1];
    DiracFrame *delay_frames[MAX_DELAY+1];
    DiracFrame all_frames[MAX_FRAMES];
} DiracContext;
