static void png_filter_row(PNGDSPContext *dsp, uint8_t *dst, int filter_type,
                           uint8_t *src, uint8_t *last, int size, int bpp)
{
    int i, p, r, g, b, a;

    switch (filter_type) {
    case PNG_FILTER_VALUE_NONE:
        memcpy(dst, src, size);
        break;
    case PNG_FILTER_VALUE_SUB:
        for (i = 0; i < bpp; i++) {
            dst[i] = src[i];
        }
        if (bpp == 4) {
            p = *(int*)dst;
            for (; i < size; i += bpp) {
                int s = *(int*)(src + i);
                p = ((s & 0x7f7f7f7f) + (p & 0x7f7f7f7f)) ^ ((s ^ p) & 0x80808080);
                *(int*)(dst + i) = p;
            }
        } else {
#define OP_SUB(x,s,l) x+s
            UNROLL_FILTER(OP_SUB);
        }
        break;
    case PNG_FILTER_VALUE_UP:
        dsp->add_bytes_l2(dst, src, last, size);
        break;
    case PNG_FILTER_VALUE_AVG:
        for (i = 0; i < bpp; i++) {
            p = (last[i] >> 1);
            dst[i] = p + src[i];
        }
#define OP_AVG(x,s,l) (((x + l) >> 1) + s) & 0xff
        UNROLL_FILTER(OP_AVG);
        break;
    case PNG_FILTER_VALUE_PAETH:
        for (i = 0; i < bpp; i++) {
            p = last[i];
            dst[i] = p + src[i];
        }
        if (bpp > 2 && size > 4) {
            // would write off the end of the array if we let it process the last pixel with bpp=3
            int w = bpp == 4 ? size : size - 3;
            dsp->add_paeth_prediction(dst + i, src + i, last + i, w - i, bpp);
            i = w;
        }
        ff_add_png_paeth_prediction(dst + i, src + i, last + i, size - i, bpp);
        break;
    }
}
