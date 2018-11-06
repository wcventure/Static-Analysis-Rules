static av_always_inline void
yuv2rgb_1_c_template(SwsContext *c, const int16_t *buf0,
                     const int16_t *ubuf[2], const int16_t *vbuf[2],
                     const int16_t *abuf0, uint8_t *dest, int dstW,
                     int uvalpha, int y, enum PixelFormat target,
                     int hasAlpha)
{
    const int16_t *ubuf0 = ubuf[0], *vbuf0 = vbuf[0];
    int i;

    if (uvalpha < 2048) {
        for (i = 0; i < (dstW >> 1); i++) {
            int Y1 = (buf0[i * 2    ] + 64) >> 7;
            int Y2 = (buf0[i * 2 + 1] + 64) >> 7;
            int U  = (ubuf0[i]        + 64) >> 7;
            int V  = (vbuf0[i]        + 64) >> 7;
            int A1, A2;
            const void *r =  c->table_rV[V + YUVRGB_TABLE_HEADROOM],
                       *g = (c->table_gU[U + YUVRGB_TABLE_HEADROOM] + c->table_gV[V + YUVRGB_TABLE_HEADROOM]),
                       *b =  c->table_bU[U + YUVRGB_TABLE_HEADROOM];

            if (hasAlpha) {
                A1 = (abuf0[i * 2    ] + 64) >> 7;
                A2 = (abuf0[i * 2 + 1] + 64) >> 7;
            }

            yuv2rgb_write(dest, i, Y1, Y2, hasAlpha ? A1 : 0, hasAlpha ? A2 : 0,
                          r, g, b, y, target, hasAlpha);
        }
    } else {
        const int16_t *ubuf1 = ubuf[1], *vbuf1 = vbuf[1];
        for (i = 0; i < (dstW >> 1); i++) {
            int Y1 = (buf0[i * 2    ]     +  64) >> 7;
            int Y2 = (buf0[i * 2 + 1]     +  64) >> 7;
            int U  = (ubuf0[i] + ubuf1[i] + 128) >> 8;
            int V  = (vbuf0[i] + vbuf1[i] + 128) >> 8;
            int A1, A2;
            const void *r =  c->table_rV[V + YUVRGB_TABLE_HEADROOM],
                       *g = (c->table_gU[U + YUVRGB_TABLE_HEADROOM] + c->table_gV[V + YUVRGB_TABLE_HEADROOM]),
                       *b =  c->table_bU[U + YUVRGB_TABLE_HEADROOM];

            if (hasAlpha) {
                A1 = (abuf0[i * 2    ] + 64) >> 7;
                A2 = (abuf0[i * 2 + 1] + 64) >> 7;
            }

            yuv2rgb_write(dest, i, Y1, Y2, hasAlpha ? A1 : 0, hasAlpha ? A2 : 0,
                          r, g, b, y, target, hasAlpha);
        }
    }
}
