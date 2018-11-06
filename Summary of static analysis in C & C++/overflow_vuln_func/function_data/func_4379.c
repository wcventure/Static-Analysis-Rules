typedef struct Plane {
    int width;
    int height;
    int stride;

    int idwt_width;
    int idwt_height;
    int idwt_stride;
    IDWTELEM *idwt_buf;
    IDWTELEM *idwt_buf_base;
    IDWTELEM *idwt_tmp;

    /
    uint8_t xblen;
    uint8_t yblen;
    /
    uint8_t xbsep;
    uint8_t ybsep;
    /
    uint8_t xoffset;
    uint8_t yoffset;

    SubBand band[MAX_DWT_LEVELS][4];
} Plane;
