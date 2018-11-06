static av_always_inline void
yuv2gray16_X_c_template(SwsContext *c, const int16_t *lumFilter,
                        const int32_t **lumSrc, int lumFilterSize,
                        const int16_t *chrFilter, const int32_t **chrUSrc,
                        const int32_t **chrVSrc, int chrFilterSize,
                        const int32_t **alpSrc, uint16_t *dest, int dstW,
                        int y, enum PixelFormat target)
{
    int i;

    for (i = 0; i < (dstW >> 1); i++) {
        int j;
        int Y1 = 1 << 14;
        int Y2 = 1 << 14;

        for (j = 0; j < lumFilterSize; j++) {
            Y1 += lumSrc[j][i * 2]     * lumFilter[j];
            Y2 += lumSrc[j][i * 2 + 1] * lumFilter[j];
        }
        Y1 >>= 15;
        Y2 >>= 15;
        if ((Y1 | Y2) & 0x10000) {
            Y1 = av_clip_uint16(Y1);
            Y2 = av_clip_uint16(Y2);
        }
        output_pixel(&dest[i * 2 + 0], Y1);
        output_pixel(&dest[i * 2 + 1], Y2);
    }
}
