int ff_get_qtpalette(int codec_id, AVIOContext *pb, uint32_t *palette)
{
    int tmp, bit_depth, color_table_id, greyscale, i;

    avio_seek(pb, 82, SEEK_CUR);

    /
    tmp = avio_rb16(pb);
    bit_depth = tmp & 0x1F;
    greyscale = tmp & 0x20;

    /
    color_table_id = avio_rb16(pb);

    /
    if (greyscale && codec_id == AV_CODEC_ID_CINEPAK)
        return 0;

    /
    if ((bit_depth == 1 || bit_depth == 2 || bit_depth == 4 || bit_depth == 8)) {
        int color_count, color_start, color_end;
        uint32_t a, r, g, b;

        /
        if (greyscale && bit_depth > 1 && color_table_id) {
            int color_index, color_dec;
            /
            color_count = 1 << bit_depth;
            color_index = 255;
            color_dec   = 256 / (color_count - 1);
            for (i = 0; i < color_count; i++) {
                r = g = b = color_index;
                palette[i] = (0xFFU << 24) | (r << 16) | (g << 8) | (b);
                color_index -= color_dec;
                if (color_index < 0)
                    color_index = 0;
            }
        } else if (color_table_id) {
            /
            const uint8_t *color_table;
            color_count = 1 << bit_depth;
            if (bit_depth == 1)
                color_table = ff_qt_default_palette_2;
            else if (bit_depth == 2)
                color_table = ff_qt_default_palette_4;
            else if (bit_depth == 4)
                color_table = ff_qt_default_palette_16;
            else
                color_table = ff_qt_default_palette_256;
            for (i = 0; i < color_count; i++) {
                r = color_table[i * 3 + 0];
                g = color_table[i * 3 + 1];
                b = color_table[i * 3 + 2];
                palette[i] = (0xFFU << 24) | (r << 16) | (g << 8) | (b);
            }
        } else {
            /
            color_start = avio_rb32(pb);
            avio_rb16(pb); /
            color_end = avio_rb16(pb);
            if ((color_start <= 255) && (color_end <= 255)) {
                for (i = color_start; i <= color_end; i++) {
                    /
                    a = avio_r8(pb);
                    avio_r8(pb);
                    r = avio_r8(pb);
                    avio_r8(pb);
                    g = avio_r8(pb);
                    avio_r8(pb);
                    b = avio_r8(pb);
                    avio_r8(pb);
                    palette[i] = (a << 24 ) | (r << 16) | (g << 8) | (b);
                }
            }
        }

        return 1;
    }

    return 0;
}
