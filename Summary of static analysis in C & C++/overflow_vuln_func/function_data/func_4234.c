static inline int draw_glyph_rgb(AVFilterBufferRef *picref, FT_Bitmap *bitmap,
                                 unsigned int x, unsigned int y,
                                 unsigned int width, unsigned int height, int pixel_step,
                                 const uint8_t rgba_color[4], const uint8_t rgba_map[4])
{
    int r, c, alpha;
    uint8_t *p;
    uint8_t src_val;

    for (r = 0; r < bitmap->rows && r+y < height; r++) {
        for (c = 0; c < bitmap->width && c+x < width; c++) {
            /
            src_val = GET_BITMAP_VAL(r, c);
            if (!src_val)
                continue;

            SET_PIXEL_RGB(picref, rgba_color, src_val, c+x, y+r, pixel_step,
                          rgba_map[0], rgba_map[1], rgba_map[2], rgba_map[3]);
        }
    }

    return 0;
}
