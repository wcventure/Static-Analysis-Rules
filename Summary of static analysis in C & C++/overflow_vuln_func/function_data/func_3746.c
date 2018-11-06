static void                                                                      \
op ## _8tap_ ## fname ## _ ## sz ## hv_ ## opt(uint8_t *dst,                     \
                                               const uint8_t *src,               \
                                               ptrdiff_t dst_stride,             \
                                               ptrdiff_t src_stride,             \
                                               int h, int mx, int my)            \
{                                                                                \
    LOCAL_ALIGNED_ ## align(uint8_t, temp, [71 * 64]);                           \
    ff_vp9_put_8tap_1d_h_ ## sz ## _ ## opt(temp, src - 3 * src_stride,          \
                                            64, src_stride,                      \
                                            h + 7,                               \
                                            ff_filters_ssse3[f][mx - 1]);        \
    ff_vp9_ ## op ## _8tap_1d_v_ ## sz ## _ ## opt(dst, temp + 3 * 64,           \
                                                   dst_stride, 64,               \
                                                   h,                            \
                                                   ff_filters_ssse3[f][my - 1]); \
}
