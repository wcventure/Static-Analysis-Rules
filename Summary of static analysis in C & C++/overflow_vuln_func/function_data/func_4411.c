static av_always_inline void
yuv2rgb48_X_c_template(SwsContext *c, const int16_t *lumFilter,
                       const int32_t **lumSrc, int lumFilterSize,
                       const int16_t *chrFilter, const int32_t **chrUSrc,
                       const int32_t **chrVSrc, int chrFilterSize,
                       const int32_t **alpSrc, uint16_t *dest, int dstW,
                       int y, enum PixelFormat target)
{
    int i;

    for (i = 0; i < ((dstW + 1) >> 1); i++) {
        int j;
        int Y1 = -0x40000000;
        int Y2 = -0x40000000;
        int U  = -128 << 23; // 19
        int V  = -128 << 23;
        int R, G, B;

        for (j = 0; j < lumFilterSize; j++) {
            Y1 += lumSrc[j][i * 2]     * lumFilter[j];
            Y2 += lumSrc[j][i * 2 + 1] * lumFilter[j];
        }
        for (j = 0; j < chrFilterSize; j++) {
            U += chrUSrc[j][i] * chrFilter[j];
            V += chrVSrc[j][i] * chrFilter[j];
        }

        // 8bit: 12+15=27; 16-bit: 12+19=31
        Y1 >>= 14; // 10
        Y1 += 0x10000;
        Y2 >>= 14;
        Y2 += 0x10000;
        U  >>= 14;
        V  >>= 14;

        // 8bit: 27 -> 17bit, 16bit: 31 - 14 = 17bit
        Y1 -= c->yuv2rgb_y_offset;
        Y2 -= c->yuv2rgb_y_offset;
        Y1 *= c->yuv2rgb_y_coeff;
        Y2 *= c->yuv2rgb_y_coeff;
        Y1 += 1 << 13; // 21
        Y2 += 1 << 13;
        // 8bit: 17 + 13bit = 30bit, 16bit: 17 + 13bit = 30bit

        R = V * c->yuv2rgb_v2r_coeff;
        G = V * c->yuv2rgb_v2g_coeff + U * c->yuv2rgb_u2g_coeff;
        B =                            U * c->yuv2rgb_u2b_coeff;

        // 8bit: 30 - 22 = 8bit, 16bit: 30bit - 14 = 16bit
        output_pixel(&dest[0], av_clip_uintp2(R_B + Y1, 30) >> 14);
        output_pixel(&dest[1], av_clip_uintp2(  G + Y1, 30) >> 14);
        output_pixel(&dest[2], av_clip_uintp2(B_R + Y1, 30) >> 14);
        output_pixel(&dest[3], av_clip_uintp2(R_B + Y2, 30) >> 14);
        output_pixel(&dest[4], av_clip_uintp2(  G + Y2, 30) >> 14);
        output_pixel(&dest[5], av_clip_uintp2(B_R + Y2, 30) >> 14);
        dest += 6;
    }
}
