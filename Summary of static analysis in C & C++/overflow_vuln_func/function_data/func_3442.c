static inline void ff_h264_weight_WxH_mmx2(uint8_t *dst, int stride, int log2_denom, int weight, int offset, int w, int h)
{
    int x, y;
    offset <<= log2_denom;
    offset += (1 << log2_denom) >> 1;
    asm volatile(
        "movd    %0, %%mm4        \n\t"
        "movd    %1, %%mm5        \n\t"
        "movd    %2, %%mm6        \n\t"
        "pshufw  $0, %%mm4, %%mm4 \n\t"
        "pshufw  $0, %%mm5, %%mm5 \n\t"
        "pxor    %%mm7, %%mm7     \n\t"
        :: "g"(weight), "g"(offset), "g"(log2_denom)
    );
    for(y=0; y<h; y+=2){
        for(x=0; x<w; x+=4){
            asm volatile(
                "movd      %0,    %%mm0 \n\t"
                "movd      %1,    %%mm1 \n\t"
                "punpcklbw %%mm7, %%mm0 \n\t"
                "punpcklbw %%mm7, %%mm1 \n\t"
                "pmullw    %%mm4, %%mm0 \n\t"
                "pmullw    %%mm4, %%mm1 \n\t"
                "paddw     %%mm5, %%mm0 \n\t"
                "paddw     %%mm5, %%mm1 \n\t"
                "psraw     %%mm6, %%mm0 \n\t"
                "psraw     %%mm6, %%mm1 \n\t"
                "packuswb  %%mm7, %%mm0 \n\t"
                "packuswb  %%mm7, %%mm1 \n\t"
                "movd      %%mm0, %0    \n\t"
                "movd      %%mm1, %1    \n\t"
                : "+m"(*(uint32_t*)(dst+x)),
                  "+m"(*(uint32_t*)(dst+x+stride))
            );
        }
        dst += 2*stride;
    }
}
