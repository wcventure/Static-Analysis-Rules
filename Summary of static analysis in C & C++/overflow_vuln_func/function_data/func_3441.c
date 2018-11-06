static void OPNAME ## h264_qpel4_h_lowpass_ ## MMX(uint8_t *dst, uint8_t *src, int dstStride, int srcStride){\
    int h=4;\
\
    asm volatile(\
        "pxor %%mm7, %%mm7          \n\t"\
        "movq %5, %%mm4             \n\t"\
        "movq %6, %%mm5             \n\t"\
        "1:                         \n\t"\
        "movd  -1(%0), %%mm1        \n\t"\
        "movd    (%0), %%mm2        \n\t"\
        "movd   1(%0), %%mm3        \n\t"\
        "movd   2(%0), %%mm0        \n\t"\
        "punpcklbw %%mm7, %%mm1     \n\t"\
        "punpcklbw %%mm7, %%mm2     \n\t"\
        "punpcklbw %%mm7, %%mm3     \n\t"\
        "punpcklbw %%mm7, %%mm0     \n\t"\
        "paddw %%mm0, %%mm1         \n\t"\
        "paddw %%mm3, %%mm2         \n\t"\
        "movd  -2(%0), %%mm0        \n\t"\
        "movd   3(%0), %%mm3        \n\t"\
        "punpcklbw %%mm7, %%mm0     \n\t"\
        "punpcklbw %%mm7, %%mm3     \n\t"\
        "paddw %%mm3, %%mm0         \n\t"\
        "psllw $2, %%mm2            \n\t"\
        "psubw %%mm1, %%mm2         \n\t"\
        "pmullw %%mm4, %%mm2        \n\t"\
        "paddw %%mm5, %%mm0         \n\t"\
        "paddw %%mm2, %%mm0         \n\t"\
        "psraw $5, %%mm0            \n\t"\
        "packuswb %%mm0, %%mm0      \n\t"\
        OP(%%mm0, (%1),%%mm6, d)\
        "add %3, %0                 \n\t"\
        "add %4, %1                 \n\t"\
        "decl %2                    \n\t"\
        " jnz 1b                    \n\t"\
        : "+a"(src), "+c"(dst), "+m"(h)\
        : "d"((long)srcStride), "S"((long)dstStride), "m"(ff_pw_5), "m"(ff_pw_16)\
        : "memory"\
    );\
}\
static void OPNAME ## h264_qpel4_v_lowpass_ ## MMX(uint8_t *dst, uint8_t *src, int dstStride, int srcStride){\
    src -= 2*srcStride;\
    asm volatile(\
        "pxor %%mm7, %%mm7          \n\t"\
        "movd (%0), %%mm0           \n\t"\
        "add %2, %0                 \n\t"\
        "movd (%0), %%mm1           \n\t"\
        "add %2, %0                 \n\t"\
        "movd (%0), %%mm2           \n\t"\
        "add %2, %0                 \n\t"\
        "movd (%0), %%mm3           \n\t"\
        "add %2, %0                 \n\t"\
        "movd (%0), %%mm4           \n\t"\
        "add %2, %0                 \n\t"\
        "punpcklbw %%mm7, %%mm0     \n\t"\
        "punpcklbw %%mm7, %%mm1     \n\t"\
        "punpcklbw %%mm7, %%mm2     \n\t"\
        "punpcklbw %%mm7, %%mm3     \n\t"\
        "punpcklbw %%mm7, %%mm4     \n\t"\
        QPEL_H264V(%%mm0, %%mm1, %%mm2, %%mm3, %%mm4, %%mm5, OP)\
        QPEL_H264V(%%mm1, %%mm2, %%mm3, %%mm4, %%mm5, %%mm0, OP)\
        QPEL_H264V(%%mm2, %%mm3, %%mm4, %%mm5, %%mm0, %%mm1, OP)\
        QPEL_H264V(%%mm3, %%mm4, %%mm5, %%mm0, %%mm1, %%mm2, OP)\
         \
        : "+a"(src), "+c"(dst)\
        : "S"((long)srcStride), "D"((long)dstStride), "m"(ff_pw_5), "m"(ff_pw_16)\
        : "memory"\
    );\
}\
static void OPNAME ## h264_qpel4_hv_lowpass_ ## MMX(uint8_t *dst, int16_t *tmp, uint8_t *src, int dstStride, int tmpStride, int srcStride){\
    int h=4;\
    int w=3;\
    src -= 2*srcStride+2;\
    while(w--){\
        asm volatile(\
            "pxor %%mm7, %%mm7      \n\t"\
            "movd (%0), %%mm0       \n\t"\
            "add %2, %0             \n\t"\
            "movd (%0), %%mm1       \n\t"\
            "add %2, %0             \n\t"\
            "movd (%0), %%mm2       \n\t"\
            "add %2, %0             \n\t"\
            "movd (%0), %%mm3       \n\t"\
            "add %2, %0             \n\t"\
            "movd (%0), %%mm4       \n\t"\
            "add %2, %0             \n\t"\
            "punpcklbw %%mm7, %%mm0 \n\t"\
            "punpcklbw %%mm7, %%mm1 \n\t"\
            "punpcklbw %%mm7, %%mm2 \n\t"\
            "punpcklbw %%mm7, %%mm3 \n\t"\
            "punpcklbw %%mm7, %%mm4 \n\t"\
            QPEL_H264HV(%%mm0, %%mm1, %%mm2, %%mm3, %%mm4, %%mm5, 0*8*3)\
            QPEL_H264HV(%%mm1, %%mm2, %%mm3, %%mm4, %%mm5, %%mm0, 1*8*3)\
            QPEL_H264HV(%%mm2, %%mm3, %%mm4, %%mm5, %%mm0, %%mm1, 2*8*3)\
            QPEL_H264HV(%%mm3, %%mm4, %%mm5, %%mm0, %%mm1, %%mm2, 3*8*3)\
             \
            : "+a"(src)\
            : "c"(tmp), "S"((long)srcStride), "m"(ff_pw_5)\
            : "memory"\
        );\
        tmp += 4;\
        src += 4 - 9*srcStride;\
    }\
    tmp -= 3*4;\
    asm volatile(\
        "movq %4, %%mm6             \n\t"\
        "1:                         \n\t"\
        "movq     (%0), %%mm0       \n\t"\
        "paddw  10(%0), %%mm0       \n\t"\
        "movq    2(%0), %%mm1       \n\t"\
        "paddw   8(%0), %%mm1       \n\t"\
        "movq    4(%0), %%mm2       \n\t"\
        "paddw   6(%0), %%mm2       \n\t"\
        "psubw %%mm1, %%mm0         \n\t"/\
        "psraw $2, %%mm0            \n\t"/\
        "psubw %%mm1, %%mm0         \n\t"/\
        "paddsw %%mm2, %%mm0        \n\t"\
        "psraw $2, %%mm0            \n\t"/\
        "paddw %%mm6, %%mm2         \n\t"\
        "paddw %%mm2, %%mm0         \n\t"\
        "psraw $6, %%mm0            \n\t"\
        "packuswb %%mm0, %%mm0      \n\t"\
        OP(%%mm0, (%1),%%mm7, d)\
        "add $24, %0                \n\t"\
        "add %3, %1                 \n\t"\
        "decl %2                    \n\t"\
        " jnz 1b                    \n\t"\
        : "+a"(tmp), "+c"(dst), "+m"(h)\
        : "S"((long)dstStride), "m"(ff_pw_32)\
        : "memory"\
    );\
}\
