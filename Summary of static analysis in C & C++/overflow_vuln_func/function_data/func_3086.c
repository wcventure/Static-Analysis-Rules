static void cdg_load_palette(CDGraphicsContext *cc, uint8_t *data, int low)
{
    uint8_t r, g, b;
    uint16_t color;
    int i;
    int array_offset  = low ? 0 : 8;
    uint32_t *palette = (uint32_t *) cc->frame.data[1];

    for (i = 0; i < 8; i++) {
        color = (data[2 * i] << 6) + (data[2 * i + 1] & 0x3F);
        r = ((color >> 8) & 0x000F) * 17;
        g = ((color >> 4) & 0x000F) * 17;
        b = ((color     ) & 0x000F) * 17;
        palette[i + array_offset] = 0xFF << 24 | r << 16 | g << 8 | b;
    }
    cc->frame.palette_has_changed = 1;
}
