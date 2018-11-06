static av_noinline void OPNAME ## h264_qpel16_h_lowpass_l2_ ## MMX(uint8_t *dst, uint8_t *src, uint8_t *src2, int dstStride, int src2Stride){\
    int h=16;\
    __asm__ volatile(\
        "pxor %%xmm15, %%xmm15      \n\t"\
        "movdqa %6, %%xmm14         \n\t"\
        "movdqa %7, %%xmm13         \n\t"\
        "1:                         \n\t"\
        "lddqu    3(%0), %%xmm1     \n\t"\
        "lddqu   -5(%0), %%xmm7     \n\t"\
        "movdqa  %%xmm1, %%xmm0     \n\t"\
        "punpckhbw %%xmm15, %%xmm1  \n\t"\
        "punpcklbw %%xmm15, %%xmm0  \n\t"\
        "punpcklbw %%xmm15, %%xmm7  \n\t"\
        "movdqa  %%xmm1, %%xmm2     \n\t"\
        "movdqa  %%xmm0, %%xmm6     \n\t"\
        "movdqa  %%xmm1, %%xmm3     \n\t"\
        "movdqa  %%xmm0, %%xmm8     \n\t"\
        "movdqa  %%xmm1, %%xmm4     \n\t"\
        "movdqa  %%xmm0, %%xmm9     \n\t"\
        "movdqa  %%xmm1, %%xmm5     \n\t"\
        "movdqa  %%xmm0, %%xmm10    \n\t"\
        "palignr $6, %%xmm0, %%xmm5 \n\t"\
        "palignr $6, %%xmm7, %%xmm10\n\t"\
        "palignr $8, %%xmm0, %%xmm4 \n\t"\
        "palignr $8, %%xmm7, %%xmm9 \n\t"\
        "palignr $10,%%xmm0, %%xmm3 \n\t"\
        "palignr $10,%%xmm7, %%xmm8 \n\t"\
        "paddw   %%xmm1, %%xmm5     \n\t"\
        "paddw   %%xmm0, %%xmm10    \n\t"\
        "palignr $12,%%xmm0, %%xmm2 \n\t"\
        "palignr $12,%%xmm7, %%xmm6 \n\t"\
        "palignr $14,%%xmm0, %%xmm1 \n\t"\
        "palignr $14,%%xmm7, %%xmm0 \n\t"\
        "paddw   %%xmm3, %%xmm2     \n\t"\
        "paddw   %%xmm8, %%xmm6     \n\t"\
        "paddw   %%xmm4, %%xmm1     \n\t"\
        "paddw   %%xmm9, %%xmm0     \n\t"\
        "psllw   $2,     %%xmm2     \n\t"\
        "psllw   $2,     %%xmm6     \n\t"\
        "psubw   %%xmm1, %%xmm2     \n\t"\
        "psubw   %%xmm0, %%xmm6     \n\t"\
        "paddw   %%xmm13,%%xmm5     \n\t"\
        "paddw   %%xmm13,%%xmm10    \n\t"\
        "pmullw  %%xmm14,%%xmm2     \n\t"\
        "pmullw  %%xmm14,%%xmm6     \n\t"\
        "lddqu   (%2),   %%xmm3     \n\t"\
        "paddw   %%xmm5, %%xmm2     \n\t"\
        "paddw   %%xmm10,%%xmm6     \n\t"\
        "psraw   $5,     %%xmm2     \n\t"\
        "psraw   $5,     %%xmm6     \n\t"\
        "packuswb %%xmm2,%%xmm6     \n\t"\
        "pavgb   %%xmm3, %%xmm6     \n\t"\
        OP(%%xmm6, (%1), %%xmm4, dqa)\
        "add %5, %0                 \n\t"\
        "add %5, %1                 \n\t"\
        "add %4, %2                 \n\t"\
        "decl %3                    \n\t"\
        "jg 1b                      \n\t"\
        : "+a"(src), "+c"(dst), "+d"(src2), "+g"(h)\
        : "D"((x86_reg)src2Stride), "S"((x86_reg)dstStride),\
          "m"(ff_pw_5), "m"(ff_pw_16)\
        : "memory"\
    );\
}
