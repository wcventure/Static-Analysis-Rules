static void type_a##_##type_b##_##sz##x##sz##_add_c(uint8_t *_dst, \
                                                    ptrdiff_t stride, \
                                                    int16_t *_block, int eob) \
{ \
    int i, j; \
    pixel *dst = (pixel *) _dst; \
    dctcoef *block = (dctcoef *) _block, tmp[sz * sz], out[sz]; \
\
    stride /= sizeof(pixel); \
    if (has_dconly && eob == 1) { \
        const int t  = (((block[0] * 11585 + (1 << 13)) >> 14) \
                                   * 11585 + (1 << 13)) >> 14; \
        block[0] = 0; \
        for (i = 0; i < sz; i++) { \
            for (j = 0; j < sz; j++) \
                dst[j * stride] = av_clip_pixel(dst[j * stride] + \
                                                (bits ? \
                                                 (t + (1 << (bits - 1))) >> bits : \
                                                 t)); \
            dst++; \
        } \
        return; \
    } \
\
    for (i = 0; i < sz; i++) \
        type_a##sz##_1d(block + i, sz, tmp + i * sz, 0); \
    memset(block, 0, sz * sz * sizeof(*block)); \
    for (i = 0; i < sz; i++) { \
        type_b##sz##_1d(tmp + i, sz, out, 1); \
        for (j = 0; j < sz; j++) \
            dst[j * stride] = av_clip_pixel(dst[j * stride] + \
                                            (bits ? \
                                             (out[j] + (1 << (bits - 1))) >> bits : \
                                             out[j])); \
        dst++; \
    } \
}
