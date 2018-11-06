static void store_slice16_c(uint16_t *dst, const uint16_t *src,
                            int dst_linesize, int src_linesize,
                            int width, int height, int log2_scale,
                            const uint8_t dither[8][8])
{
    int y, x;

#define STORE16(pos) do {                                                   \
    temp = ((src[x + y*src_linesize + pos] << log2_scale) + (d[pos]>>1)) >> 5;   \
    if (temp & 0x400)                                                       \
        temp = ~(temp >> 31);                                               \
    dst[x + y*dst_linesize + pos] = temp;                                   \
} while (0)

    for (y = 0; y < height; y++) {
        const uint8_t *d = dither[y];
        for (x = 0; x < width; x += 8) {
            int temp;
            STORE16(0);
            STORE16(1);
            STORE16(2);
            STORE16(3);
            STORE16(4);
            STORE16(5);
            STORE16(6);
            STORE16(7);
        }
    }
}
