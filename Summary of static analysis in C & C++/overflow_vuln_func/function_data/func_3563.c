typedef struct DNXHDContext {
    AVCodecContext *avctx;
    RowContext *rows;
    BlockDSPContext bdsp;
    const uint8_t* buf;
    int buf_size;
    int64_t cid;                        ///< compression id
    unsigned int width, height;
    enum AVPixelFormat pix_fmt;
    unsigned int mb_width, mb_height;
    uint32_t mb_scan_index[68];         /
    int cur_field;                      ///< current interlaced field
    VLC ac_vlc, dc_vlc, run_vlc;
    IDCTDSPContext idsp;
    ScanTable scantable;
    const CIDEntry *cid_table;
    int bit_depth; // 8, 10 or 0 if not initialized at all.
    int is_444;
    int mbaff;
    int act;
    void (*decode_dct_block)(const struct DNXHDContext *ctx,
                             RowContext *row, int n);
} DNXHDContext;
