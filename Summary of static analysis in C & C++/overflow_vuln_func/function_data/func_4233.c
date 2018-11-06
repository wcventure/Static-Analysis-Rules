static inline int draw_glyph_yuv(AVFilterBufferRef *picref, FT_Bitmap *bitmap, unsigned int x,
                                 unsigned int y, unsigned int width, unsigned int height,
                                 const uint8_t yuva_color[4], int hsub, int vsub)
{
    int r, c, alpha;
    unsigned int luma_pos, chroma_pos1, chroma_pos2;
    uint8_t src_val;

    for (r = 0; r < bitmap->rows && r+y < height; r++) {
        for (c = 0; c < bitmap->width && c+x < width; c++) {
            /
            src_val = GET_BITMAP_VAL(r, c);
            if (!src_val)
                continue;

            SET_PIXEL_YUV(picref, yuva_color, src_val, c+x, y+r, hsub, vsub);
        }
    }

    return 0;
}
