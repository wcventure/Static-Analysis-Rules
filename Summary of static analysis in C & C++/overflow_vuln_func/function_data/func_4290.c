static av_always_inline void rgb16_32ToUV_half_c_template(int16_t *dstU,
                                                          int16_t *dstV,
                                                          const uint8_t *src,
                                                          int width,
                                                          enum AVPixelFormat origin,
                                                          int shr, int shg,
                                                          int shb, int shp,
                                                          int maskr, int maskg,
                                                          int maskb, int rsh,
                                                          int gsh, int bsh, int S,
                                                          int32_t *rgb2yuv)
{
    const int ru       = rgb2yuv[RU_IDX] << rsh, gu = rgb2yuv[GU_IDX] << gsh, bu = rgb2yuv[BU_IDX] << bsh,
              rv       = rgb2yuv[RV_IDX] << rsh, gv = rgb2yuv[GV_IDX] << gsh, bv = rgb2yuv[BV_IDX] << bsh,
              maskgx   = ~(maskr | maskb);
    const unsigned rnd = (256U<<(S)) + (1<<(S-6));
    int i;

    maskr |= maskr << 1;
    maskb |= maskb << 1;
    maskg |= maskg << 1;
    for (i = 0; i < width; i++) {
        int px0 = input_pixel(2 * i + 0) >> shp;
        int px1 = input_pixel(2 * i + 1) >> shp;
        int b, r, g = (px0 & maskgx) + (px1 & maskgx);
        int rb = px0 + px1 - g;

        b = (rb & maskb) >> shb;
        if (shp ||
            origin == AV_PIX_FMT_BGR565LE || origin == AV_PIX_FMT_BGR565BE ||
            origin == AV_PIX_FMT_RGB565LE || origin == AV_PIX_FMT_RGB565BE) {
            g >>= shg;
        } else {
            g = (g & maskg) >> shg;
        }
        r = (rb & maskr) >> shr;

        dstU[i] = (ru * r + gu * g + bu * b + (unsigned)rnd) >> ((S)-6+1);
        dstV[i] = (rv * r + gv * g + bv * b + (unsigned)rnd) >> ((S)-6+1);
    }
}
