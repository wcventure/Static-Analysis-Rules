static void put_signed_rect_clamped_ ## PX ## bit_c(uint8_t *_dst, int dst_stride, const uint8_t *_src, \
                                                  int src_stride, int width, int height)                \
{                                                                                                       \
    int x, y;                                                                                           \
    uint16_t *dst = (uint16_t *)_dst;                                                                   \
    int32_t *src = (int32_t *)_src;                                                                     \
    for (y = 0; y < height; y++) {                                                                      \
        for (x = 0; x < width; x+=4) {                                                                  \
            dst[x  ] = av_clip_uintp2(src[x  ] + (1 << (PX - 1)), PX);                                  \
            dst[x+1] = av_clip_uintp2(src[x+1] + (1 << (PX - 1)), PX);                                  \
            dst[x+2] = av_clip_uintp2(src[x+2] + (1 << (PX - 1)), PX);                                  \
            dst[x+3] = av_clip_uintp2(src[x+3] + (1 << (PX - 1)), PX);                                  \
        }                                                                                               \
        dst += dst_stride >> 1;                                                                         \
        src += src_stride >> 2;                                                                         \
    }                                                                                                   \
}
