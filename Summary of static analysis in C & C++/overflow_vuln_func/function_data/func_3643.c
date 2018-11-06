static void guess_palette(DVDSubContext* ctx,
                          uint32_t *rgba_palette,
                          uint32_t subtitle_color)
{
    static const uint8_t level_map[4][4] = {
        // this configuration (full range, lowest to highest) in tests
        // seemed most common, so assume this
        {0xff},
        {0x00, 0xff},
        {0x00, 0x80, 0xff},
        {0x00, 0x55, 0xaa, 0xff},
    };
    uint8_t color_used[16] = { 0 };
    int nb_opaque_colors, i, level, j, r, g, b;
    uint8_t *colormap = ctx->colormap, *alpha = ctx->alpha;

    if(ctx->has_palette) {
        for(i = 0; i < 4; i++)
            rgba_palette[i] = (ctx->palette[colormap[i]] & 0x00ffffff)
                              | ((alpha[i] * 17U) << 24);
        return;
    }

    for(i = 0; i < 4; i++)
        rgba_palette[i] = 0;

    nb_opaque_colors = 0;
    for(i = 0; i < 4; i++) {
        if (alpha[i] != 0 && !color_used[colormap[i]]) {
            color_used[colormap[i]] = 1;
            nb_opaque_colors++;
        }
    }

    if (nb_opaque_colors == 0)
        return;

    j = 0;
    memset(color_used, 0, 16);
    for(i = 0; i < 4; i++) {
        if (alpha[i] != 0) {
            if (!color_used[colormap[i]])  {
                level = level_map[nb_opaque_colors][j];
                r = (((subtitle_color >> 16) & 0xff) * level) >> 8;
                g = (((subtitle_color >> 8) & 0xff) * level) >> 8;
                b = (((subtitle_color >> 0) & 0xff) * level) >> 8;
                rgba_palette[i] = b | (g << 8) | (r << 16) | ((alpha[i] * 17) << 24);
                color_used[colormap[i]] = (i + 1);
                j++;
            } else {
                rgba_palette[i] = (rgba_palette[color_used[colormap[i]] - 1] & 0x00ffffff) |
                                    ((alpha[i] * 17) << 24);
            }
        }
    }
}
