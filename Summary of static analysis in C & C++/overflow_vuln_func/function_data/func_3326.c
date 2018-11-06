    for (i = 0; i < height; i++) { \
        const uint8_t *dither = dithers[i & 7]; \
        for (j = 0; j < length - 7; j += 8) { \
            wfunc(&dst[j + 0], (rfunc(&src[j + 0]) + dither[0]) >> shift); \
            wfunc(&dst[j + 1], (rfunc(&src[j + 1]) + dither[1]) >> shift); \
            wfunc(&dst[j + 2], (rfunc(&src[j + 2]) + dither[2]) >> shift); \
            wfunc(&dst[j + 3], (rfunc(&src[j + 3]) + dither[3]) >> shift); \
            wfunc(&dst[j + 4], (rfunc(&src[j + 4]) + dither[4]) >> shift); \
            wfunc(&dst[j + 5], (rfunc(&src[j + 5]) + dither[5]) >> shift); \
            wfunc(&dst[j + 6], (rfunc(&src[j + 6]) + dither[6]) >> shift); \
            wfunc(&dst[j + 7], (rfunc(&src[j + 7]) + dither[7]) >> shift); \
        } \
        for (; j < length; j++) \
            wfunc(&dst[j],     (rfunc(&src[j]) + dither[j & 7]) >> shift); \
        dst += dstStride; \
        src += srcStride; \
    }
