static void hScale16To15_c(SwsContext *c, int16_t *dst, int dstW, const uint8_t *_src,
                           const int16_t *filter,
                           const int16_t *filterPos, int filterSize)
{
    int i;
    const uint16_t *src = (const uint16_t *) _src;
    int sh = av_pix_fmt_descriptors[c->srcFormat].comp[0].depth_minus1;

    for (i = 0; i < dstW; i++) {
        int j;
        int srcPos = filterPos[i];
        int val = 0;

        for (j = 0; j < filterSize; j++) {
            val += src[srcPos + j] * filter[filterSize * i + j];
        }
        // filter=14 bit, input=16 bit, output=30 bit, >> 15 makes 15 bit
        dst[i] = FFMIN(val >> sh, (1 << 15) - 1);
    }
}
