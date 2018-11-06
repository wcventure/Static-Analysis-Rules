static av_always_inline void
rgb16_32ToY_c_template(uint8_t *dst, const uint8_t *src,
                       int width, enum PixelFormat origin,
                       int shr,   int shg,   int shb, int shp,
                       int maskr, int maskg, int maskb,
                       int rsh,   int gsh,   int bsh, int S)
{
    const int ry = RY << rsh, gy = GY << gsh, by = BY << bsh,
              rnd = 33 << (S - 1);
    int i;

    for (i = 0; i < width; i++) {
        int px = input_pixel(i) >> shp;
        int b = (px & maskb) >> shb;
        int g = (px & maskg) >> shg;
        int r = (px & maskr) >> shr;

        dst[i] = (ry * r + gy * g + by * b + rnd) >> S;
    }
}
