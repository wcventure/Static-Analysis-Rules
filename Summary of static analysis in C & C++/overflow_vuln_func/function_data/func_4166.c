typedef struct Indeo3DecodeContext {
    AVCodecContext *avctx;
    HpelDSPContext  hdsp;

    GetBitContext   gb;
    int             need_resync;
    int             skip_bits;
    const uint8_t   *next_cell_data;
    const uint8_t   *last_byte;
    const int8_t    *mc_vectors;
    unsigned        num_vectors;    ///< number of motion vectors in mc_vectors

    int16_t         width, height;
    uint32_t        frame_num;      ///< current frame number (zero-based)
    uint32_t        data_size;      ///< size of the frame data in bytes
    uint16_t        frame_flags;    ///< frame properties
    uint8_t         cb_offset;      ///< needed for selecting VQ tables
    uint8_t         buf_sel;        ///< active frame buffer: 0 - primary, 1 -secondary
    const uint8_t   *y_data_ptr;
    const uint8_t   *v_data_ptr;
    const uint8_t   *u_data_ptr;
    int32_t         y_data_size;
    int32_t         v_data_size;
    int32_t         u_data_size;
    const uint8_t   *alt_quant;     ///< secondary VQ table set for the modes 1 and 4
    Plane           planes[3];
} Indeo3DecodeContext;
