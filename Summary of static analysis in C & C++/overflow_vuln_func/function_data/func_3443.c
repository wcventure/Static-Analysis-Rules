static inline void ff_h264_biweight_WxH_mmx2(uint8_t *dst, uint8_t *src, int stride, int log2_denom, int weightd, int weights, int offsetd, int offsets, int w, int h)
{
    int x, y;
    int offset = ((offsets + offsetd + 1) | 1) << log2_denom;
    asm volatile(
        "movd    %0, %%mm3        \n\t"
        "movd    %1, %%mm4        \n\t"
        "movd    %2, %%mm5        \n\t"
        "movd    %3, %%mm6        \n\t"
        "pshufw  $0, %%mm3, %%mm3 \n\t"
        "pshufw  $0, %%mm4, %%mm4 \n\t"
        "pshufw  $0, %%mm5, %%mm5 \n\t"
        "pxor    %%mm7, %%mm7     \n\t"
        :: "g"(weightd), "g"(weights), "g"(offset), "g"(log2_denom+1)
    );
    for(y=0; y<h; y++){
        for(x=0; x<w; x+=4){
            asm volatile(
                "movd      %0,    %%mm0 \n\t"
                "movd      %1,    %%mm1 \n\t"
                "punpcklbw %%mm7, %%mm0 \n\t"
                "punpcklbw %%mm7, %%mm1 \n\t"
                "pmullw    %%mm3, %%mm0 \n\t"
                "pmullw    %%mm4, %%mm1 \n\t"
                "paddw     %%mm5, %%mm0 \n\t"
                "paddw     %%mm1, %%mm0 \n\t"
                "psraw     %%mm6, %%mm0 \n\t"
                "packuswb  %%mm0, %%mm0 \n\t"
                "movd      %%mm0, %0    \n\t"
                : "+m"(*(uint32_t*)(dst+x))
                :  "m"(*(uint32_t*)(src+x))
            );
        }
        src += stride;
        dst += stride;
    }
}
