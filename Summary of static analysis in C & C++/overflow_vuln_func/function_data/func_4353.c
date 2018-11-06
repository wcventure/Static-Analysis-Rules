static void store_slice_mmx(uint8_t *dst, const uint16_t *src,
                            int dst_stride, int src_stride,
                            int width, int height, int log2_scale,
                            const uint8_t dither[8][8])
{
    int y;

    for (y = 0; y < height; y++) {
        uint8_t *dst1 = dst;
        const int16_t *src1 = src;
        __asm__ volatile(
            "movq (%3), %%mm3           \n"
            "movq (%3), %%mm4           \n"
            "movd %4, %%mm2             \n"
            "pxor %%mm0, %%mm0          \n"
            "punpcklbw %%mm0, %%mm3     \n"
            "punpckhbw %%mm0, %%mm4     \n"
            "psraw %%mm2, %%mm3         \n"
            "psraw %%mm2, %%mm4         \n"
            "movd %5, %%mm2             \n"
            "1:                         \n"
            "movq (%0), %%mm0           \n"
            "movq 8(%0), %%mm1          \n"
            "paddw %%mm3, %%mm0         \n"
            "paddw %%mm4, %%mm1         \n"
            "psraw %%mm2, %%mm0         \n"
            "psraw %%mm2, %%mm1         \n"
            "packuswb %%mm1, %%mm0      \n"
            "movq %%mm0, (%1)           \n"
            "add $16, %0                \n"
            "add $8, %1                 \n"
            "cmp %2, %1                 \n"
            " jb 1b                     \n"
            : "+r" (src1), "+r"(dst1)
            : "r"(dst + width), "r"(dither[y]), "g"(log2_scale), "g"(MAX_LEVEL - log2_scale)
        );
        src += src_stride;
        dst += dst_stride;
    }
}
