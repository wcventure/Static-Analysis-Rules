static int parse_palette(AVCodecContext *avctx, GetByteContext *gbc,
                         uint32_t *pal, int colors)
{
    int i;

    for (i = 0; i <= colors; i++) {
        uint8_t r, g, b;
        unsigned int idx = bytestream2_get_be16(gbc); /
        if (idx > 255) {
            av_log(avctx, AV_LOG_WARNING,
                   "Palette index out of range: %u\n", idx);
            bytestream2_skip(gbc, 6);
            continue;
        }
        r = bytestream2_get_byte(gbc);
        bytestream2_skip(gbc, 1);
        g = bytestream2_get_byte(gbc);
        bytestream2_skip(gbc, 1);
        b = bytestream2_get_byte(gbc);
        bytestream2_skip(gbc, 1);
        pal[idx] = (r << 16) | (g << 8) | b;
    }
    return 0;
}
