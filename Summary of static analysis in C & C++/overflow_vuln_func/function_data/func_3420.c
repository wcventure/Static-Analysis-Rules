static av_always_inline void
yuv2rgb48_2_c_template(SwsContext *c, const int32_t *buf[2],
                       const int32_t *ubuf[2], const int32_t *vbuf[2],
                       const int32_t *abuf[2], uint16_t *dest, int dstW,
                       int yalpha, int uvalpha, int y,
                       enum PixelFormat target)
{
    const int32_t *buf0  = buf[0],  *buf1  = buf[1],
                  *ubuf0 = ubuf[0], *ubuf1 = ubuf[1],
                  *vbuf0 = vbuf[0], *vbuf1 = vbuf[1];
    int  yalpha1 = 4095 - yalpha;
    int uvalpha1 = 4095 - uvalpha;
    int i;

    for (i = 0; i < ((dstW + 1) >> 1); i++) {
        int Y1 = (buf0[i * 2]     * yalpha1  + buf1[i * 2]     * yalpha) >> 14;
        int Y2 = (buf0[i * 2 + 1] * yalpha1  + buf1[i * 2 + 1] * yalpha) >> 14;
        int U  = (ubuf0[i]        * uvalpha1 + ubuf1[i]        * uvalpha + (-128 << 23)) >> 14;
        int V  = (vbuf0[i]        * uvalpha1 + vbuf1[i]        * uvalpha + (-128 << 23)) >> 14;
        int R, G, B;

        Y1 -= c->yuv2rgb_y_offset;
        Y2 -= c->yuv2rgb_y_offset;
        Y1 *= c->yuv2rgb_y_coeff;
        Y2 *= c->yuv2rgb_y_coeff;
        Y1 += 1 << 13;
        Y2 += 1 << 13;

        R = V * c->yuv2rgb_v2r_coeff;
        G = V * c->yuv2rgb_v2g_coeff + U * c->yuv2rgb_u2g_coeff;
        B =                            U * c->yuv2rgb_u2b_coeff;

        output_pixel(&dest[0], av_clip_uintp2(R_B + Y1, 30) >> 14);
        output_pixel(&dest[1], av_clip_uintp2(  G + Y1, 30) >> 14);
        output_pixel(&dest[2], av_clip_uintp2(B_R + Y1, 30) >> 14);
        output_pixel(&dest[3], av_clip_uintp2(R_B + Y2, 30) >> 14);
        output_pixel(&dest[4], av_clip_uintp2(  G + Y2, 30) >> 14);
        output_pixel(&dest[5], av_clip_uintp2(B_R + Y2, 30) >> 14);
        dest += 6;
    }
}
