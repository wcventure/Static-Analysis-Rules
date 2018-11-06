static void dequant_subband_ ## PX ## _c(uint8_t *src, uint8_t *dst, ptrdiff_t stride,     \
                                         const int qf, const int qs, int tot_v, int tot_h) \
{                                                                                          \
    int i, y;                                                                              \
    for (y = 0; y < tot_v; y++) {                                                          \
        PX c, sign, *src_r = (PX *)src, *dst_r = (PX *)dst;                                \
        for (i = 0; i < tot_h; i++) {                                                      \
            c = *src_r++;                                                                  \
            sign = FFSIGN(c)*(!!c);                                                        \
            c = (FFABS(c)*qf + qs) >> 2;                                                   \
            *dst_r++ = c*sign;                                                             \
        }                                                                                  \
        src += tot_h << (sizeof(PX) >> 1);                                                 \
        dst += stride;                                                                     \
    }                                                                                      \
}
