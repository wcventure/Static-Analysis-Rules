static inline void drawbox(AVFilterBufferRef *picref, unsigned int x, unsigned int y,
                           unsigned int width, unsigned int height,
                           uint8_t *line[4], int pixel_step[4], uint8_t color[4],
                           int hsub, int vsub, int is_rgba_packed, uint8_t rgba_map[4])
{
    int i, j, alpha;

    if (color[3] != 0xFF) {
        if (is_rgba_packed) {
            uint8_t *p;
            for (j = 0; j < height; j++)
                for (i = 0; i < width; i++)
                    SET_PIXEL_RGB(picref, color, 255, i+x, y+j, pixel_step[0],
                                  rgba_map[0], rgba_map[1], rgba_map[2], rgba_map[3]);
        } else {
            unsigned int luma_pos, chroma_pos1, chroma_pos2;
            for (j = 0; j < height; j++)
                for (i = 0; i < width; i++)
                    SET_PIXEL_YUV(picref, color, 255, i+x, y+j, hsub, vsub);
        }
    } else {
        ff_draw_rectangle(picref->data, picref->linesize,
                          line, pixel_step, hsub, vsub,
                          x, y, width, height);
    }
}
