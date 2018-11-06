static av_always_inline void
yuv2yuvX16_c_template(const int16_t *lumFilter, const int32_t **lumSrc,
                      int lumFilterSize, const int16_t *chrFilter,
                      const int32_t **chrUSrc, const int32_t **chrVSrc,
                      int chrFilterSize, const int32_t **alpSrc,
                      uint16_t *dest[4], int dstW, int chrDstW,
                      int big_endian, int output_bits)
{
    //FIXME Optimize (just quickly written not optimized..)
    int i;
    uint16_t *yDest = dest[0], *uDest = dest[1], *vDest = dest[2],
             *aDest = CONFIG_SWSCALE_ALPHA ? dest[3] : NULL;
    int shift = 15 + 16 - output_bits;

#define output_pixel(pos, val) \
    if (big_endian) { \
        if (output_bits == 16) { \
            AV_WB16(pos, av_clip_uint16(val >> shift)); \
        } else { \
            AV_WB16(pos, av_clip_uintp2(val >> shift, output_bits)); \
        } \
    } else { \
        if (output_bits == 16) { \
            AV_WL16(pos, av_clip_uint16(val >> shift)); \
        } else { \
            AV_WL16(pos, av_clip_uintp2(val >> shift, output_bits)); \
        } \
    }
    for (i = 0; i < dstW; i++) {
        int val = 1 << (30-output_bits);
        int j;

        for (j = 0; j < lumFilterSize; j++)
            val += lumSrc[j][i] * lumFilter[j];

        output_pixel(&yDest[i], val);
    }

    if (uDest) {
        for (i = 0; i < chrDstW; i++) {
            int u = 1 << (30-output_bits);
            int v = 1 << (30-output_bits);
            int j;

            for (j = 0; j < chrFilterSize; j++) {
                u += chrUSrc[j][i] * chrFilter[j];
                v += chrVSrc[j][i] * chrFilter[j];
            }

            output_pixel(&uDest[i], u);
            output_pixel(&vDest[i], v);
        }
    }

    if (CONFIG_SWSCALE_ALPHA && aDest) {
        for (i = 0; i < dstW; i++) {
            int val = 1 << (30-output_bits);
            int j;

            for (j = 0; j < lumFilterSize; j++)
                val += alpSrc[j][i] * lumFilter[j];

            output_pixel(&aDest[i], val);
        }
    }
#undef output_pixel
}
