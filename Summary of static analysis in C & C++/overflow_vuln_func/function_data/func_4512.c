static av_always_inline void
rgb16_32ToUV_c_template(uint8_t *dstU, uint8_t *dstV,
                        const uint8_t *src, int width,
                        enum PixelFormat origin,
                        int shr,   int shg,   int shb, int shp,
                        int maskr, int maskg, int maskb,
                        int rsh,   int gsh,   int bsh, int S)
{
    const int ru = RU << rsh, gu = GU << gsh, bu = BU << bsh,
              rv = RV << rsh, gv = GV << gsh, bv = BV << bsh,
              rnd = 257 << (S - 1);
    int i;

    for (i = 0; i < width; i++) {
        int px = input_pixel(i) >> shp;
        int b = (px & maskb) >> shb;
        int g = (px & maskg) >> shg;
        int r = (px & maskr) >> shr;

        dstU[i] = (ru * r + gu * g + bu * b + rnd) >> S;
        dstV[i] = (rv * r + gv * g + bv * b + rnd) >> S;
    }
}
