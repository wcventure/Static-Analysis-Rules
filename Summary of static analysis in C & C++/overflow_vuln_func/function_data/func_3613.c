static av_noinline void OPNAME ## h264_qpel8_h_lowpass_l2_ ## MMX(uint8_t *dst, uint8_t *src, uint8_t *src2, int dstStride, int src2Stride){\
    int h=8;\
    __asm__ volatile(\
        "pxor %%xmm7, %%xmm7        \n\t"\
        "movdqa %0, %%xmm6          \n\t"\
        :: "m"(ff_pw_5)\
    );\
    do{\
    __asm__ volatile(\
        "lddqu   -5(%0), %%xmm1     \n\t"\
        "movdqa  %%xmm1, %%xmm0     \n\t"\
        "punpckhbw %%xmm7, %%xmm1   \n\t"\
        "punpcklbw %%xmm7, %%xmm0   \n\t"\
        "movdqa  %%xmm1, %%xmm2     \n\t"\
        "movdqa  %%xmm1, %%xmm3     \n\t"\
        "movdqa  %%xmm1, %%xmm4     \n\t"\
        "movdqa  %%xmm1, %%xmm5     \n\t"\
        "palignr $6, %%xmm0, %%xmm5 \n\t"\
        "palignr $8, %%xmm0, %%xmm4 \n\t"\
        "palignr $10,%%xmm0, %%xmm3 \n\t"\
        "paddw   %%xmm1, %%xmm5     \n\t"\
        "palignr $12,%%xmm0, %%xmm2 \n\t"\
        "palignr $14,%%xmm0, %%xmm1 \n\t"\
        "paddw   %%xmm3, %%xmm2     \n\t"\
        "paddw   %%xmm4, %%xmm1     \n\t"\
        "psllw   $2,     %%xmm2     \n\t"\
        "movq    (%2),   %%xmm3     \n\t"\
        "psubw   %%xmm1, %%xmm2     \n\t"\
        "paddw   %5,     %%xmm5     \n\t"\
        "pmullw  %%xmm6, %%xmm2     \n\t"\
        "paddw   %%xmm5, %%xmm2     \n\t"\
        "psraw   $5,     %%xmm2     \n\t"\
        "packuswb %%xmm2, %%xmm2    \n\t"\
        "pavgb   %%xmm3, %%xmm2     \n\t"\
        OP(%%xmm2, (%1), %%xmm4, q)\
        "add %4, %0                 \n\t"\
        "add %4, %1                 \n\t"\
        "add %3, %2                 \n\t"\
        : "+a"(src), "+c"(dst), "+d"(src2)\
        : "D"((x86_reg)src2Stride), "S"((x86_reg)dstStride),\
          "m"(ff_pw_16)\
        : "memory"\
    );\
    }while(--h);\
}\
