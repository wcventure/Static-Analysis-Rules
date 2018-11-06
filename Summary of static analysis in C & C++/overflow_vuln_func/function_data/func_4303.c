static inline void render_line_unrolled(intptr_t x, uint8_t y, int x1,
                                        intptr_t sy, int ady, int adx,
                                        float *buf)
{
    int err = -adx;
    x -= x1 - 1;
    buf += x1 - 1;
    while (++x < 0) {
        err += ady;
        if (err >= 0) {
            err += ady - adx;
            y   += sy;
            buf[x++] = ff_vorbis_floor1_inverse_db_table[y];
        }
        buf[x] = ff_vorbis_floor1_inverse_db_table[y];
    }
    if (x <= 0) {
        if (err + ady >= 0)
            y += sy;
        buf[x] = ff_vorbis_floor1_inverse_db_table[y];
    }
}
