static inline void rgtc_block_internal(uint8_t *dst, ptrdiff_t stride,
                                       const uint8_t *block,
                                       const int *color_tab)
{
    uint8_t indices[16];
    int x, y;

    decompress_indices(indices, block + 2);

    /
    for (y = 0; y < 4; y++) {
        for (x = 0; x < 4; x++) {
            int i = indices[x + y * 4];
            /
            int c = color_tab[i];
            uint32_t pixel = RGBA(c, c, c, 255);
            AV_WL32(dst + x * 4 + y * stride, pixel);
        }
    }
}
