static av_always_inline void
yuv2rgb_write(uint8_t *_dest, int i, int Y1, int Y2,
              int U, int V, int A1, int A2,
              const void *_r, const void *_g, const void *_b, int y,
              enum PixelFormat target, int hasAlpha)
{
    if (target == PIX_FMT_ARGB || target == PIX_FMT_RGBA ||
        target == PIX_FMT_ABGR || target == PIX_FMT_BGRA) {
        uint32_t *dest = (uint32_t *) _dest;
        const uint32_t *r = (const uint32_t *) _r;
        const uint32_t *g = (const uint32_t *) _g;
        const uint32_t *b = (const uint32_t *) _b;

#if CONFIG_SMALL
        int sh = hasAlpha ? ((target == PIX_FMT_RGB32_1 || target == PIX_FMT_BGR32_1) ? 0 : 24) : 0;

        dest[i * 2 + 0] = r[Y1] + g[Y1] + b[Y1] + (hasAlpha ? A1 << sh : 0);
        dest[i * 2 + 1] = r[Y2] + g[Y2] + b[Y2] + (hasAlpha ? A2 << sh : 0);
#else
        if (hasAlpha) {
            int sh = (target == PIX_FMT_RGB32_1 || target == PIX_FMT_BGR32_1) ? 0 : 24;

            dest[i * 2 + 0] = r[Y1] + g[Y1] + b[Y1] + (A1 << sh);
            dest[i * 2 + 1] = r[Y2] + g[Y2] + b[Y2] + (A2 << sh);
        } else {
            dest[i * 2 + 0] = r[Y1] + g[Y1] + b[Y1];
            dest[i * 2 + 1] = r[Y2] + g[Y2] + b[Y2];
        }
#endif
    } else if (target == PIX_FMT_RGB24 || target == PIX_FMT_BGR24) {
        uint8_t *dest = (uint8_t *) _dest;
        const uint8_t *r = (const uint8_t *) _r;
        const uint8_t *g = (const uint8_t *) _g;
        const uint8_t *b = (const uint8_t *) _b;

#define r_b ((target == PIX_FMT_RGB24) ? r : b)
#define b_r ((target == PIX_FMT_RGB24) ? b : r)
        dest[i * 6 + 0] = r_b[Y1];
        dest[i * 6 + 1] =   g[Y1];
        dest[i * 6 + 2] = b_r[Y1];
        dest[i * 6 + 3] = r_b[Y2];
        dest[i * 6 + 4] =   g[Y2];
        dest[i * 6 + 5] = b_r[Y2];
#undef r_b
#undef b_r
    } else if (target == PIX_FMT_RGB565 || target == PIX_FMT_BGR565 ||
               target == PIX_FMT_RGB555 || target == PIX_FMT_BGR555 ||
               target == PIX_FMT_RGB444 || target == PIX_FMT_BGR444) {
        uint16_t *dest = (uint16_t *) _dest;
        const uint16_t *r = (const uint16_t *) _r;
        const uint16_t *g = (const uint16_t *) _g;
        const uint16_t *b = (const uint16_t *) _b;
        int dr1, dg1, db1, dr2, dg2, db2;

        if (target == PIX_FMT_RGB565 || target == PIX_FMT_BGR565) {
            dr1 = dither_2x2_8[ y & 1     ][0];
            dg1 = dither_2x2_4[ y & 1     ][0];
            db1 = dither_2x2_8[(y & 1) ^ 1][0];
            dr2 = dither_2x2_8[ y & 1     ][1];
            dg2 = dither_2x2_4[ y & 1     ][1];
            db2 = dither_2x2_8[(y & 1) ^ 1][1];
        } else if (target == PIX_FMT_RGB555 || target == PIX_FMT_BGR555) {
            dr1 = dither_2x2_8[ y & 1     ][0];
            dg1 = dither_2x2_8[ y & 1     ][1];
            db1 = dither_2x2_8[(y & 1) ^ 1][0];
            dr2 = dither_2x2_8[ y & 1     ][1];
            dg2 = dither_2x2_8[ y & 1     ][0];
            db2 = dither_2x2_8[(y & 1) ^ 1][1];
        } else {
            dr1 = dither_4x4_16[ y & 3     ][0];
            dg1 = dither_4x4_16[ y & 3     ][1];
            db1 = dither_4x4_16[(y & 3) ^ 3][0];
            dr2 = dither_4x4_16[ y & 3     ][1];
            dg2 = dither_4x4_16[ y & 3     ][0];
            db2 = dither_4x4_16[(y & 3) ^ 3][1];
        }

        dest[i * 2 + 0] = r[Y1 + dr1] + g[Y1 + dg1] + b[Y1 + db1];
        dest[i * 2 + 1] = r[Y2 + dr2] + g[Y2 + dg2] + b[Y2 + db2];
    } else / {
        uint8_t *dest = (uint8_t *) _dest;
        const uint8_t *r = (const uint8_t *) _r;
        const uint8_t *g = (const uint8_t *) _g;
        const uint8_t *b = (const uint8_t *) _b;
        int dr1, dg1, db1, dr2, dg2, db2;

        if (target == PIX_FMT_RGB8 || target == PIX_FMT_BGR8) {
            const uint8_t * const d64 = dither_8x8_73[y & 7];
            const uint8_t * const d32 = dither_8x8_32[y & 7];
            dr1 = dg1 = d32[(i * 2 + 0) & 7];
            db1 =       d64[(i * 2 + 0) & 7];
            dr2 = dg2 = d32[(i * 2 + 1) & 7];
            db2 =       d64[(i * 2 + 1) & 7];
        } else {
            const uint8_t * const d64  = dither_8x8_73 [y & 7];
            const uint8_t * const d128 = dither_8x8_220[y & 7];
            dr1 = db1 = d128[(i * 2 + 0) & 7];
            dg1 =        d64[(i * 2 + 0) & 7];
            dr2 = db2 = d128[(i * 2 + 1) & 7];
            dg2 =        d64[(i * 2 + 1) & 7];
        }

        if (target == PIX_FMT_RGB4 || target == PIX_FMT_BGR4) {
            dest[i] = r[Y1 + dr1] + g[Y1 + dg1] + b[Y1 + db1] +
                    ((r[Y2 + dr2] + g[Y2 + dg2] + b[Y2 + db2]) << 4);
        } else {
            dest[i * 2 + 0] = r[Y1 + dr1] + g[Y1 + dg1] + b[Y1 + db1];
            dest[i * 2 + 1] = r[Y2 + dr2] + g[Y2 + dg2] + b[Y2 + db2];
        }
    }
}
