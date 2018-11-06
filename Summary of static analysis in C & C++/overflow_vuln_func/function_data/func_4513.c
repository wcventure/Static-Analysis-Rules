static av_always_inline void
rgb16_32ToUV_half_c_template(uint8_t *dstU, uint8_t *dstV,
                             const uint8_t *src, int width,
                             enum PixelFormat origin,
                             int shr,   int shg,   int shb, int shp,
                             int maskr, int maskg, int maskb,
                             int rsh,   int gsh,   int bsh, int S)
{
    const int ru = RU << rsh, gu = GU << gsh, bu = BU << bsh,
              rv = RV << rsh, gv = GV << gsh, bv = BV << bsh,
              rnd = 257 << S, maskgx = ~(maskr | maskb);
    int i;

    maskr |= maskr << 1; maskb |= maskb << 1; maskg |= maskg << 1;
    for (i = 0; i < width; i++) {
        int px0 = input_pixel(2 * i + 0) >> shp;
        int px1 = input_pixel(2 * i + 1) >> shp;
        int b, r, g = (px0 & maskgx) + (px1 & maskgx);
        int rb = px0 + px1 - g;

        b = (rb & maskb) >> shb;
        if (shp || origin == PIX_FMT_BGR565LE || origin == PIX_FMT_BGR565BE ||
            origin == PIX_FMT_RGB565LE || origin == PIX_FMT_RGB565BE) {
            g >>= shg;
        } else {
            g = (g  & maskg) >> shg;
        }
        r = (rb & maskr) >> shr;

        dstU[i] = (ru * r + gu * g + bu * b + rnd) >> (S + 1);
        dstV[i] = (rv * r + gv * g + bv * b + rnd) >> (S + 1);
    }
}
