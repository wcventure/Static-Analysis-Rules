#if COMPILE_TEMPLATE_MMX2
static void RENAME(hyscale_fast)(SwsContext *c, int16_t *dst,
                                 int dstWidth, const uint8_t *src,
                                 int srcW, int xInc)
{
    int16_t *filterPos = c->hLumFilterPos;
    int16_t *filter    = c->hLumFilter;
    void    *mmx2FilterCode= c->lumMmx2FilterCode;
    int i;
#if defined(PIC)
    uint64_t ebxsave;
#endif
#if ARCH_X86_64
    uint64_t retsave;
#endif

    __asm__ volatile(
#if defined(PIC)
        "mov               %%"REG_b", %5        \n\t"
#if ARCH_X86_64
        "mov               -8(%%rsp), %%"REG_a" \n\t"
        "mov               %%"REG_a", %6        \n\t"
#endif
#else
#if ARCH_X86_64
        "mov               -8(%%rsp), %%"REG_a" \n\t"
        "mov               %%"REG_a", %5        \n\t"
#endif
#endif
        "pxor                  %%mm7, %%mm7     \n\t"
        "mov                      %0, %%"REG_c" \n\t"
        "mov                      %1, %%"REG_D" \n\t"
        "mov                      %2, %%"REG_d" \n\t"
        "mov                      %3, %%"REG_b" \n\t"
        "xor               %%"REG_a", %%"REG_a" \n\t" // i
        PREFETCH"        (%%"REG_c")            \n\t"
        PREFETCH"      32(%%"REG_c")            \n\t"
        PREFETCH"      64(%%"REG_c")            \n\t"

#if ARCH_X86_64
#define CALL_MMX2_FILTER_CODE \
        "movl            (%%"REG_b"), %%esi     \n\t"\
        "call                    *%4            \n\t"\
        "movl (%%"REG_b", %%"REG_a"), %%esi     \n\t"\
        "add               %%"REG_S", %%"REG_c" \n\t"\
        "add               %%"REG_a", %%"REG_D" \n\t"\
        "xor               %%"REG_a", %%"REG_a" \n\t"\

#else
#define CALL_MMX2_FILTER_CODE \
        "movl (%%"REG_b"), %%esi        \n\t"\
        "call         *%4                       \n\t"\
        "addl (%%"REG_b", %%"REG_a"), %%"REG_c" \n\t"\
        "add               %%"REG_a", %%"REG_D" \n\t"\
        "xor               %%"REG_a", %%"REG_a" \n\t"\

#endif /

        CALL_MMX2_FILTER_CODE
        CALL_MMX2_FILTER_CODE
        CALL_MMX2_FILTER_CODE
        CALL_MMX2_FILTER_CODE
        CALL_MMX2_FILTER_CODE
        CALL_MMX2_FILTER_CODE
        CALL_MMX2_FILTER_CODE
        CALL_MMX2_FILTER_CODE

#if defined(PIC)
        "mov                      %5, %%"REG_b" \n\t"
#if ARCH_X86_64
        "mov                      %6, %%"REG_a" \n\t"
        "mov               %%"REG_a", -8(%%rsp) \n\t"
#endif
#else
#if ARCH_X86_64
        "mov                      %5, %%"REG_a" \n\t"
        "mov               %%"REG_a", -8(%%rsp) \n\t"
#endif
#endif
        :: "m" (src), "m" (dst), "m" (filter), "m" (filterPos),
           "m" (mmx2FilterCode)
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

    for (i=dstWidth-1; (i*xInc)>>16 >=srcW-1; i--)
        dst[i] = src[srcW-1]*128;
}
