static void RENAME(hcscale_fast)(SwsContext *c, int16_t *dst1, int16_t *dst2,
                                 int dstWidth, const uint8_t *src1,
                                 const uint8_t *src2, int srcW, int xInc)
{
    int16_t *filterPos = c->hChrFilterPos;
    int16_t *filter    = c->hChrFilter;
    void    *mmx2FilterCode= c->chrMmx2FilterCode;
    int i;
#if defined(PIC)
    DECLARE_ALIGNED(8, uint64_t, ebxsave);
#endif
#if ARCH_X86_64
    DECLARE_ALIGNED(8, uint64_t, retsave);
#endif

    __asm__ volatile(
#if defined(PIC)
        "mov          %%"REG_b", %7         \n\t"
#if ARCH_X86_64
        "mov          -8(%%rsp), %%"REG_a"  \n\t"
        "mov          %%"REG_a", %8         \n\t"
#endif
#else
#if ARCH_X86_64
        "mov          -8(%%rsp), %%"REG_a"  \n\t"
        "mov          %%"REG_a", %7         \n\t"
#endif
#endif
        "pxor             %%mm7, %%mm7      \n\t"
        "mov                 %0, %%"REG_c"  \n\t"
        "mov                 %1, %%"REG_D"  \n\t"
        "mov                 %2, %%"REG_d"  \n\t"
        "mov                 %3, %%"REG_b"  \n\t"
        "xor          %%"REG_a", %%"REG_a"  \n\t" // i
        PREFETCH"   (%%"REG_c")             \n\t"
        PREFETCH" 32(%%"REG_c")             \n\t"
        PREFETCH" 64(%%"REG_c")             \n\t"

        CALL_MMX2_FILTER_CODE
        CALL_MMX2_FILTER_CODE
        CALL_MMX2_FILTER_CODE
        CALL_MMX2_FILTER_CODE
        "xor          %%"REG_a", %%"REG_a"  \n\t" // i
        "mov                 %5, %%"REG_c"  \n\t" // src
        "mov                 %6, %%"REG_D"  \n\t" // buf2
        PREFETCH"   (%%"REG_c")             \n\t"
        PREFETCH" 32(%%"REG_c")             \n\t"
        PREFETCH" 64(%%"REG_c")             \n\t"

        CALL_MMX2_FILTER_CODE
        CALL_MMX2_FILTER_CODE
        CALL_MMX2_FILTER_CODE
        CALL_MMX2_FILTER_CODE

#if defined(PIC)
        "mov %7, %%"REG_b"    \n\t"
#if ARCH_X86_64
        "mov                 %8, %%"REG_a"  \n\t"
        "mov          %%"REG_a", -8(%%rsp)  \n\t"
#endif
#else
#if ARCH_X86_64
        "mov                 %7, %%"REG_a"  \n\t"
        "mov          %%"REG_a", -8(%%rsp)  \n\t"
#endif
#endif
        :: "m" (src1), "m" (dst1), "m" (filter), "m" (filterPos),
           "m" (mmx2FilterCode), "m" (src2), "m"(dst2)
#if defined(PIC)
          ,"m" (ebxsave)
#endif
#if ARCH_X86_64
          ,"m"(retsave)
#endif
        : "%"REG_a, "%"REG_c, "%"REG_d, "%"REG_S, "%"REG_D
#if !defined(PIC)
         ,"%"REG_b
#endif
    );

    for (i=dstWidth-1; (i*xInc)>>16 >=srcW-1; i--) {
        dst1[i] = src1[srcW-1]*128;
        dst2[i] = src2[srcW-1]*128;
    }
}
