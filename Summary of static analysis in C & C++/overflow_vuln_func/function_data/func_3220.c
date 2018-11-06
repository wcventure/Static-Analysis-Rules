typedef struct TTAContext {
    AVCodecContext *avctx;
    AVFrame frame;
    GetBitContext gb;
    const AVCRC *crc_table;

    int format, channels, bps, data_length;
    int frame_length, last_frame_length, total_frames;

    int32_t *decode_buffer;

    TTAChannel *ch_ctx;
} TTAContext;
