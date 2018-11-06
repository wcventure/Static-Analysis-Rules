static inline void dxt1_decode_pixels(const uint8_t *s, uint32_t *d,
                                      unsigned int qstride, unsigned int flag,
                                      uint64_t alpha) {
    unsigned int x, y, c0, c1, a = (!flag * 255) << 24;
    unsigned int rb0, rb1, rb2, rb3, g0, g1, g2, g3;
    uint32_t colors[4], pixels;

    c0 = AV_RL16(s);
    c1 = AV_RL16(s+2);

    rb0  = (c0<<3 | c0<<8) & 0xf800f8;
    rb1  = (c1<<3 | c1<<8) & 0xf800f8;
    rb0 +=        (rb0>>5) & 0x070007;
    rb1 +=        (rb1>>5) & 0x070007;
    g0   =        (c0 <<5) & 0x00fc00;
    g1   =        (c1 <<5) & 0x00fc00;
    g0  +=        (g0 >>6) & 0x000300;
    g1  +=        (g1 >>6) & 0x000300;

    colors[0] = rb0 + g0 + a;
    colors[1] = rb1 + g1 + a;

    if (c0 > c1 || flag) {
        rb2 = (((2*rb0+rb1) * 21) >> 6) & 0xff00ff;
        rb3 = (((2*rb1+rb0) * 21) >> 6) & 0xff00ff;
        g2  = (((2*g0 +g1 ) * 21) >> 6) & 0x00ff00;
        g3  = (((2*g1 +g0 ) * 21) >> 6) & 0x00ff00;
        colors[3] = rb3 + g3 + a;
    } else {
        rb2 = ((rb0+rb1) >> 1) & 0xff00ff;
        g2  = ((g0 +g1 ) >> 1) & 0x00ff00;
        colors[3] = 0;
    }

    colors[2] = rb2 + g2 + a;

    pixels = AV_RL32(s+4);
    for (y=0; y<4; y++) {
        for (x=0; x<4; x++) {
            a        = (alpha & 0x0f) << 28;
            a       += a >> 4;
            d[x]     = a + colors[pixels&3];
            pixels >>= 2;
            alpha  >>= 4;
        }
        d += qstride;
    }
}
