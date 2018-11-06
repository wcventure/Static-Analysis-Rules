                                                                        \
    static void                                                         \
    tight_encode_indexed_rect##bpp(uint8_t *buf, int count,             \
                                   VncPalette *palette) {               \
        uint##bpp##_t *src;                                             \
        uint##bpp##_t rgb;                                              \
        int i, rep;                                                     \
        uint8_t idx;                                                    \
                                                                        \
        src = (uint##bpp##_t *) buf;                                    \
                                                                        \
        for (i = 0; i < count; i++) {                                   \
                                                                        \
            rgb = *src++;                                               \
            rep = 0;                                                    \
            while (i < count && *src == rgb) {                          \
                rep++, src++, i++;                                      \
            }                                                           \
            idx = palette_idx(palette, rgb);                            \
            /                                                         \
            if (idx == (uint8_t)-1) {                                   \
                idx = 0;                                                \
            }                                                           \
            while (rep >= 0) {                                          \
                *buf++ = idx;                                           \
                rep--;                                                  \
            }                                                           \
        }                                                               \
    }
