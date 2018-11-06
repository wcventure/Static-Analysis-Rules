typedef struct MmContext {
    AVCodecContext *avctx;
    AVFrame *frame;
    int palette[AVPALETTE_COUNT];
    GetByteContext gb;
} MmContext;
