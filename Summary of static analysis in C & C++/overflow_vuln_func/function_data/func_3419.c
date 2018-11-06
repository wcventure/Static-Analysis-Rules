static av_always_inline void
yuv2422_2_c_template(SwsContext *c, const int16_t *buf[2],
                     const int16_t *ubuf[2], const int16_t *vbuf[2],
                     const int16_t *abuf[2], uint8_t *dest, int dstW,
                     int yalpha, int uvalpha, int y,
                     enum PixelFormat target)
{
    const int16_t *buf0  = buf[0],  *buf1  = buf[1],
                  *ubuf0 = ubuf[0], *ubuf1 = ubuf[1],
                  *vbuf0 = vbuf[0], *vbuf1 = vbuf[1];
    int  yalpha1 = 4095 - yalpha;
    int uvalpha1 = 4095 - uvalpha;
    int i;

    for (i = 0; i < ((dstW + 1) >> 1); i++) {
        int Y1 = (buf0[i * 2]     * yalpha1  + buf1[i * 2]     * yalpha)  >> 19;
        int Y2 = (buf0[i * 2 + 1] * yalpha1  + buf1[i * 2 + 1] * yalpha)  >> 19;
        int U  = (ubuf0[i]        * uvalpha1 + ubuf1[i]        * uvalpha) >> 19;
        int V  = (vbuf0[i]        * uvalpha1 + vbuf1[i]        * uvalpha) >> 19;

        Y1 = av_clip_uint8(Y1);
        Y2 = av_clip_uint8(Y2);
        U  = av_clip_uint8(U);
        V  = av_clip_uint8(V);

        output_pixels(i * 4, Y1, U, Y2, V);
    }
}
