static void store_slice_c(uint8_t *dst, const int16_t *src,
                          int dst_stride, int src_stride,
                          int width, int height, int log2_scale)
{
    int y, x;

#define STORE(pos) do {                                                     \
    temp = ((src[x + y * src_stride + pos] << log2_scale) + d[pos]) >> 8;   \
    if (temp & 0x100) temp = ~(temp >> 31);                                 \
    dst[x + y * dst_stride + pos] = temp;                                   \
} while (0)

    for (y = 0; y < height; y++) {
        const uint8_t *d = dither[y&7];
        for (x = 0; x < width; x += 8) {
            int temp;
            STORE(0);
            STORE(1);
            STORE(2);
            STORE(3);
            STORE(4);
            STORE(5);
            STORE(6);
            STORE(7);
        }
    }
}
