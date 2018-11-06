typedef struct {
    const AVClass *av_class;
    int log2_count;
    int qp;
    int qscale_type;
    int temp_stride[3];
    uint8_t *src[3];
    int16_t *temp[3];
    int outbuf_size;
    uint8_t *outbuf;
    AVCodecContext *avctx_enc[BLOCK*BLOCK];
    AVFrame *frame;
    AVFrame *frame_dec;
    uint8_t *non_b_qp_table;
    int non_b_qp_alloc_size;
    int use_bframe_qp;
} USPPContext;
