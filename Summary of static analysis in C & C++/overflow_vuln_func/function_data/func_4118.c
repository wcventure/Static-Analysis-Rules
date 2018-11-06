static av_always_inline void
yuv2mono_X_c_template(SwsContext *c, const int16_t *lumFilter,
                      const int16_t **lumSrc, int lumFilterSize,
                      const int16_t *chrFilter, const int16_t **chrUSrc,
                      const int16_t **chrVSrc, int chrFilterSize,
                      const int16_t **alpSrc, uint8_t *dest, int dstW,
                      int y, enum PixelFormat target)
{
    const uint8_t * const d128=dither_8x8_220[y&7];
    uint8_t *g = c->table_gU[128] + c->table_gV[128];
    int i;
    int acc = 0;

    for (i = 0; i < dstW - 1; i += 2) {
        int j;
        int Y1 = 1 << 18;
        int Y2 = 1 << 18;

        for (j = 0; j < lumFilterSize; j++) {
            Y1 += lumSrc[j][i]   * lumFilter[j];
            Y2 += lumSrc[j][i+1] * lumFilter[j];
        }
        Y1 >>= 19;
        Y2 >>= 19;
        if ((Y1 | Y2) & 0x100) {
            Y1 = av_clip_uint8(Y1);
            Y2 = av_clip_uint8(Y2);
        }
        acc += acc + g[Y1 + d128[(i + 0) & 7]];
        acc += acc + g[Y2 + d128[(i + 1) & 7]];
        if ((i & 7) == 6) {
            output_pixel(*dest++, acc);
        }
    }
}
