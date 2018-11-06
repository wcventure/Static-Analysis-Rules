static void vga_putcharxy(DisplayState *ds, int x, int y, int ch,
                          TextAttributes *t_attrib)
{
    uint8_t *d;
    const uint8_t *font_ptr;
    unsigned int font_data, linesize, xorcol, bpp;
    int i;
    unsigned int fgcol, bgcol;

#ifdef DEBUG_CONSOLE
    printf("x: %2i y: %2i", x, y);
    console_print_text_attributes(t_attrib, ch);
#endif

    if (t_attrib->invers) {
        bgcol = color_table[t_attrib->bold][t_attrib->fgcol];
        fgcol = color_table[t_attrib->bold][t_attrib->bgcol];
    } else {
        fgcol = color_table[t_attrib->bold][t_attrib->fgcol];
        bgcol = color_table[t_attrib->bold][t_attrib->bgcol];
    }

    bpp = (ds_get_bits_per_pixel(ds) + 7) >> 3;
    d = ds_get_data(ds) +
        ds_get_linesize(ds) * y * FONT_HEIGHT + bpp * x * FONT_WIDTH;
    linesize = ds_get_linesize(ds);
    font_ptr = vgafont16 + FONT_HEIGHT * ch;
    xorcol = bgcol ^ fgcol;
    switch(ds_get_bits_per_pixel(ds)) {
    case 8:
        for(i = 0; i < FONT_HEIGHT; i++) {
            font_data = *font_ptr++;
            if (t_attrib->uline
                && ((i == FONT_HEIGHT - 2) || (i == FONT_HEIGHT - 3))) {
                font_data = 0xFFFF;
            }
            ((uint32_t *)d)[0] = (dmask16[(font_data >> 4)] & xorcol) ^ bgcol;
            ((uint32_t *)d)[1] = (dmask16[(font_data >> 0) & 0xf] & xorcol) ^ bgcol;
            d += linesize;
        }
        break;
    case 16:
    case 15:
        for(i = 0; i < FONT_HEIGHT; i++) {
            font_data = *font_ptr++;
            if (t_attrib->uline
                && ((i == FONT_HEIGHT - 2) || (i == FONT_HEIGHT - 3))) {
                font_data = 0xFFFF;
            }
            ((uint32_t *)d)[0] = (dmask4[(font_data >> 6)] & xorcol) ^ bgcol;
            ((uint32_t *)d)[1] = (dmask4[(font_data >> 4) & 3] & xorcol) ^ bgcol;
            ((uint32_t *)d)[2] = (dmask4[(font_data >> 2) & 3] & xorcol) ^ bgcol;
            ((uint32_t *)d)[3] = (dmask4[(font_data >> 0) & 3] & xorcol) ^ bgcol;
            d += linesize;
        }
        break;
    case 32:
        for(i = 0; i < FONT_HEIGHT; i++) {
            font_data = *font_ptr++;
            if (t_attrib->uline && ((i == FONT_HEIGHT - 2) || (i == FONT_HEIGHT - 3))) {
                font_data = 0xFFFF;
            }
            ((uint32_t *)d)[0] = (-((font_data >> 7)) & xorcol) ^ bgcol;
            ((uint32_t *)d)[1] = (-((font_data >> 6) & 1) & xorcol) ^ bgcol;
            ((uint32_t *)d)[2] = (-((font_data >> 5) & 1) & xorcol) ^ bgcol;
            ((uint32_t *)d)[3] = (-((font_data >> 4) & 1) & xorcol) ^ bgcol;
            ((uint32_t *)d)[4] = (-((font_data >> 3) & 1) & xorcol) ^ bgcol;
            ((uint32_t *)d)[5] = (-((font_data >> 2) & 1) & xorcol) ^ bgcol;
            ((uint32_t *)d)[6] = (-((font_data >> 1) & 1) & xorcol) ^ bgcol;
            ((uint32_t *)d)[7] = (-((font_data >> 0) & 1) & xorcol) ^ bgcol;
            d += linesize;
        }
        break;
    }
}
