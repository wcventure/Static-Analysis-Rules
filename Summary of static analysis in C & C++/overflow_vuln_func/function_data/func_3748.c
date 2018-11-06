av_cold void ff_vp9dsp_init_x86(VP9DSPContext *dsp)
{
#if HAVE_YASM
    int cpu_flags = av_get_cpu_flags();

#define init_fpel(idx1, idx2, sz, type, opt)                            \
    dsp->mc[idx1][FILTER_8TAP_SMOOTH ][idx2][0][0] =                    \
    dsp->mc[idx1][FILTER_8TAP_REGULAR][idx2][0][0] =                    \
    dsp->mc[idx1][FILTER_8TAP_SHARP  ][idx2][0][0] =                    \
    dsp->mc[idx1][FILTER_BILINEAR    ][idx2][0][0] = ff_vp9_ ## type ## sz ## _ ## opt


#define init_subpel1(idx1, idx2, idxh, idxv, sz, dir, type, opt) \
    dsp->mc[idx1][FILTER_8TAP_SMOOTH][idx2][idxh][idxv]  = type ## _8tap_smooth_  ## sz ## dir ## _ ## opt; \
    dsp->mc[idx1][FILTER_8TAP_REGULAR][idx2][idxh][idxv] = type ## _8tap_regular_ ## sz ## dir ## _ ## opt; \
    dsp->mc[idx1][FILTER_8TAP_SHARP][idx2][idxh][idxv]   = type ## _8tap_sharp_   ## sz ## dir ## _ ## opt

#define init_subpel2_32_64(idx, idxh, idxv, dir, type, opt) \
    init_subpel1(0, idx, idxh, idxv, 64, dir, type, opt);   \
    init_subpel1(1, idx, idxh, idxv, 32, dir, type, opt)

#define init_subpel2(idx, idxh, idxv, dir, type, opt)     \
    init_subpel2_32_64(idx, idxh, idxv, dir, type, opt);  \
    init_subpel1(2, idx, idxh, idxv, 16, dir, type, opt); \
    init_subpel1(3, idx, idxh, idxv,  8, dir, type, opt); \
    init_subpel1(4, idx, idxh, idxv,  4, dir, type, opt)

#define init_subpel3(idx, type, opt)        \
    init_subpel2(idx, 1, 1, hv, type, opt); \
    init_subpel2(idx, 0, 1,  v, type, opt); \
    init_subpel2(idx, 1, 0,  h, type, opt)

    if (EXTERNAL_MMX(cpu_flags)) {
        init_fpel(4, 0,  4, put, mmx);
        init_fpel(3, 0,  8, put, mmx);
    }

    if (EXTERNAL_MMXEXT(cpu_flags)) {
        init_fpel(4, 1,  4, avg, mmxext);
        init_fpel(3, 1,  8, avg, mmxext);
    }

    if (EXTERNAL_SSE(cpu_flags)) {
        init_fpel(2, 0, 16, put, sse);
        init_fpel(1, 0, 32, put, sse);
        init_fpel(0, 0, 64, put, sse);
    }

    if (EXTERNAL_SSE2(cpu_flags)) {
        init_fpel(2, 1, 16, avg, sse2);
        init_fpel(1, 1, 32, avg, sse2);
        init_fpel(0, 1, 64, avg, sse2);
    }

    if (EXTERNAL_SSSE3(cpu_flags)) {
        init_subpel3(0, put, ssse3);
        init_subpel3(1, avg, ssse3);
    }

    if (EXTERNAL_AVX(cpu_flags)) {
        init_fpel(1, 0, 32, put, avx);
        init_fpel(0, 0, 64, put, avx);
    }

    if (EXTERNAL_AVX2(cpu_flags)) {
        init_fpel(1, 1, 32, avg, avx2);
        init_fpel(0, 1, 64, avg, avx2);

#if ARCH_X86_64 && HAVE_AVX2_EXTERNAL
        init_subpel2_32_64(0, 1, 1, hv, put, avx2);
        init_subpel2_32_64(0, 0, 1, v,  put, avx2);
        init_subpel2_32_64(0, 1, 0, h,  put, avx2);
        init_subpel2_32_64(1, 1, 1, hv, avg, avx2);
        init_subpel2_32_64(1, 0, 1, v,  avg, avx2);
        init_subpel2_32_64(1, 1, 0, h,  avg, avx2);
#endif /
    }

#undef init_fpel
#undef init_subpel1
#undef init_subpel2
#undef init_subpel3

#endif /
}
