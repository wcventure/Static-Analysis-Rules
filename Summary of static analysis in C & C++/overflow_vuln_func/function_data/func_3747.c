static void                                                                \
op ## _8tap_ ## fname ## _ ## sz ## dir ## _ ## opt(uint8_t *dst,          \
                                                    const uint8_t *src,    \
                                                    ptrdiff_t dst_stride,  \
                                                    ptrdiff_t src_stride,  \
                                                    int h, int mx,         \
                                                    int my)                \
{                                                                          \
    ff_vp9_ ## op ## _8tap_1d_ ## dir ## _ ## sz ## _ ## opt(dst, src,     \
                                                             dst_stride,   \
                                                             src_stride, h,\
                                                             ff_filters_ssse3[f][dvar - 1]); \
}
