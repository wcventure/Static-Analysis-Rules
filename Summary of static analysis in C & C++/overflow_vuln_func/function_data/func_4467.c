av_cold void ff_vp9dsp_init_x86(VP9DSPContext *dsp, int bpp)
{
#if HAVE_YASM
    int cpu_flags;
    if (bpp != 8) return;

    cpu_flags = av_get_cpu_flags();

#define init_fpel(idx1, idx2, sz, type, opt) \
    dsp->mc[idx1][FILTER_8TAP_SMOOTH ][idx2][0][0] = \
    dsp->mc[idx1][FILTER_8TAP_REGULAR][idx2][0][0] = \
    dsp->mc[idx1][FILTER_8TAP_SHARP  ][idx2][0][0] = \
    dsp->mc[idx1][FILTER_BILINEAR    ][idx2][0][0] = ff_vp9_##type##sz##_##opt

#define init_subpel1(idx1, idx2, idxh, idxv, sz, dir, type, opt) \
    dsp->mc[idx1][FILTER_8TAP_SMOOTH ][idx2][idxh][idxv] = type##_8tap_smooth_##sz##dir##_##opt; \
    dsp->mc[idx1][FILTER_8TAP_REGULAR][idx2][idxh][idxv] = type##_8tap_regular_##sz##dir##_##opt; \
    dsp->mc[idx1][FILTER_8TAP_SHARP  ][idx2][idxh][idxv] = type##_8tap_sharp_##sz##dir##_##opt

#define init_subpel2(idx1, idx2, sz, type, opt) \
    init_subpel1(idx1, idx2, 1, 1, sz, hv, type, opt); \
    init_subpel1(idx1, idx2, 0, 1, sz, v,  type, opt); \
    init_subpel1(idx1, idx2, 1, 0, sz, h,  type, opt)

#define init_subpel3_32_64(idx, type, opt) \
    init_subpel2(0, idx, 64, type, opt); \
    init_subpel2(1, idx, 32, type, opt)

#define init_subpel3_8to64(idx, type, opt) \
    init_subpel3_32_64(idx, type, opt); \
    init_subpel2(2, idx, 16, type, opt); \
    init_subpel2(3, idx,  8, type, opt)

#define init_subpel3(idx, type, opt) \
    init_subpel3_8to64(idx, type, opt); \
    init_subpel2(4, idx,  4, type, opt)

#define init_lpf(opt) do { \
    dsp->loop_filter_16[0] = ff_vp9_loop_filter_h_16_16_##opt; \
    dsp->loop_filter_16[1] = ff_vp9_loop_filter_v_16_16_##opt; \
    dsp->loop_filter_mix2[0][0][0] = ff_vp9_loop_filter_h_44_16_##opt; \
    dsp->loop_filter_mix2[0][0][1] = ff_vp9_loop_filter_v_44_16_##opt; \
    dsp->loop_filter_mix2[0][1][0] = ff_vp9_loop_filter_h_48_16_##opt; \
    dsp->loop_filter_mix2[0][1][1] = ff_vp9_loop_filter_v_48_16_##opt; \
    dsp->loop_filter_mix2[1][0][0] = ff_vp9_loop_filter_h_84_16_##opt; \
    dsp->loop_filter_mix2[1][0][1] = ff_vp9_loop_filter_v_84_16_##opt; \
    dsp->loop_filter_mix2[1][1][0] = ff_vp9_loop_filter_h_88_16_##opt; \
    dsp->loop_filter_mix2[1][1][1] = ff_vp9_loop_filter_v_88_16_##opt; \
} while (0)

#define init_ipred(sz, opt, t, e) \
    dsp->intra_pred[TX_##sz##X##sz][e##_PRED] = ff_vp9_ipred_##t##_##sz##x##sz##_##opt

#define ff_vp9_ipred_hd_4x4_ssse3 ff_vp9_ipred_hd_4x4_mmxext
#define ff_vp9_ipred_vl_4x4_ssse3 ff_vp9_ipred_vl_4x4_mmxext
#define init_dir_tm_ipred(sz, opt) do { \
    init_ipred(sz, opt, dl, DIAG_DOWN_LEFT); \
    init_ipred(sz, opt, dr, DIAG_DOWN_RIGHT); \
    init_ipred(sz, opt, hd, HOR_DOWN); \
    init_ipred(sz, opt, vl, VERT_LEFT); \
    init_ipred(sz, opt, hu, HOR_UP); \
    init_ipred(sz, opt, tm, TM_VP8); \
    init_ipred(sz, opt, vr, VERT_RIGHT); \
} while (0)
#define init_dir_tm_h_ipred(sz, opt) do { \
    init_dir_tm_ipred(sz, opt); \
    init_ipred(sz, opt, h,  HOR); \
} while (0)
#define init_dc_ipred(sz, opt) do { \
    init_ipred(sz, opt, dc,      DC); \
    init_ipred(sz, opt, dc_left, LEFT_DC); \
    init_ipred(sz, opt, dc_top,  TOP_DC); \
} while (0)
#define init_all_ipred(sz, opt) do { \
    init_dc_ipred(sz, opt); \
    init_dir_tm_h_ipred(sz, opt); \
} while (0)

    if (EXTERNAL_MMX(cpu_flags)) {
        init_fpel(4, 0,  4, put, mmx);
        init_fpel(3, 0,  8, put, mmx);
        dsp->itxfm_add[4 /][DCT_DCT] =
        dsp->itxfm_add[4 /][ADST_DCT] =
        dsp->itxfm_add[4 /][DCT_ADST] =
        dsp->itxfm_add[4 /][ADST_ADST] = ff_vp9_iwht_iwht_4x4_add_mmx;
        init_ipred(8, mmx, v, VERT);
    }

    if (EXTERNAL_MMXEXT(cpu_flags)) {
        init_subpel2(4, 0, 4, put, mmxext);
        init_subpel2(4, 1, 4, avg, mmxext);
        init_fpel(4, 1,  4, avg, mmxext);
        init_fpel(3, 1,  8, avg, mmxext);
        dsp->itxfm_add[TX_4X4][DCT_DCT] = ff_vp9_idct_idct_4x4_add_mmxext;
        init_dc_ipred(4, mmxext);
        init_dc_ipred(8, mmxext);
        init_dir_tm_ipred(4, mmxext);
    }

    if (EXTERNAL_SSE(cpu_flags)) {
        init_fpel(2, 0, 16, put, sse);
        init_fpel(1, 0, 32, put, sse);
        init_fpel(0, 0, 64, put, sse);
        init_ipred(16, sse, v, VERT);
        init_ipred(32, sse, v, VERT);
    }

    if (EXTERNAL_SSE2(cpu_flags)) {
        init_subpel3_8to64(0, put, sse2);
        init_subpel3_8to64(1, avg, sse2);
        init_fpel(2, 1, 16, avg, sse2);
        init_fpel(1, 1, 32, avg, sse2);
        init_fpel(0, 1, 64, avg, sse2);
        init_lpf(sse2);
        dsp->itxfm_add[TX_4X4][ADST_DCT]  = ff_vp9_idct_iadst_4x4_add_sse2;
        dsp->itxfm_add[TX_4X4][DCT_ADST]  = ff_vp9_iadst_idct_4x4_add_sse2;
        dsp->itxfm_add[TX_4X4][ADST_ADST] = ff_vp9_iadst_iadst_4x4_add_sse2;
        dsp->itxfm_add[TX_8X8][DCT_DCT] = ff_vp9_idct_idct_8x8_add_sse2;
        dsp->itxfm_add[TX_8X8][ADST_DCT]  = ff_vp9_idct_iadst_8x8_add_sse2;
        dsp->itxfm_add[TX_8X8][DCT_ADST]  = ff_vp9_iadst_idct_8x8_add_sse2;
        dsp->itxfm_add[TX_8X8][ADST_ADST] = ff_vp9_iadst_iadst_8x8_add_sse2;
        dsp->itxfm_add[TX_16X16][DCT_DCT]   = ff_vp9_idct_idct_16x16_add_sse2;
        dsp->itxfm_add[TX_16X16][ADST_DCT]  = ff_vp9_idct_iadst_16x16_add_sse2;
        dsp->itxfm_add[TX_16X16][DCT_ADST]  = ff_vp9_iadst_idct_16x16_add_sse2;
        dsp->itxfm_add[TX_16X16][ADST_ADST] = ff_vp9_iadst_iadst_16x16_add_sse2;
        dsp->itxfm_add[TX_32X32][ADST_ADST] =
        dsp->itxfm_add[TX_32X32][ADST_DCT] =
        dsp->itxfm_add[TX_32X32][DCT_ADST] =
        dsp->itxfm_add[TX_32X32][DCT_DCT] = ff_vp9_idct_idct_32x32_add_sse2;
        init_dc_ipred(16, sse2);
        init_dc_ipred(32, sse2);
        init_dir_tm_h_ipred(8, sse2);
        init_dir_tm_h_ipred(16, sse2);
        init_dir_tm_h_ipred(32, sse2);
        init_ipred(4, sse2, h, HOR);
    }

    if (EXTERNAL_SSSE3(cpu_flags)) {
        init_subpel3(0, put, ssse3);
        init_subpel3(1, avg, ssse3);
        dsp->itxfm_add[TX_4X4][DCT_DCT] = ff_vp9_idct_idct_4x4_add_ssse3;
        dsp->itxfm_add[TX_4X4][ADST_DCT]  = ff_vp9_idct_iadst_4x4_add_ssse3;
        dsp->itxfm_add[TX_4X4][DCT_ADST]  = ff_vp9_iadst_idct_4x4_add_ssse3;
        dsp->itxfm_add[TX_4X4][ADST_ADST] = ff_vp9_iadst_iadst_4x4_add_ssse3;
        dsp->itxfm_add[TX_8X8][DCT_DCT] = ff_vp9_idct_idct_8x8_add_ssse3;
        dsp->itxfm_add[TX_8X8][ADST_DCT]  = ff_vp9_idct_iadst_8x8_add_ssse3;
        dsp->itxfm_add[TX_8X8][DCT_ADST]  = ff_vp9_iadst_idct_8x8_add_ssse3;
        dsp->itxfm_add[TX_8X8][ADST_ADST] = ff_vp9_iadst_iadst_8x8_add_ssse3;
        dsp->itxfm_add[TX_16X16][DCT_DCT]   = ff_vp9_idct_idct_16x16_add_ssse3;
        dsp->itxfm_add[TX_16X16][ADST_DCT]  = ff_vp9_idct_iadst_16x16_add_ssse3;
        dsp->itxfm_add[TX_16X16][DCT_ADST]  = ff_vp9_iadst_idct_16x16_add_ssse3;
        dsp->itxfm_add[TX_16X16][ADST_ADST] = ff_vp9_iadst_iadst_16x16_add_ssse3;
        dsp->itxfm_add[TX_32X32][ADST_ADST] =
        dsp->itxfm_add[TX_32X32][ADST_DCT] =
        dsp->itxfm_add[TX_32X32][DCT_ADST] =
        dsp->itxfm_add[TX_32X32][DCT_DCT] = ff_vp9_idct_idct_32x32_add_ssse3;
        init_lpf(ssse3);
        init_all_ipred(4, ssse3);
        init_all_ipred(8, ssse3);
        init_all_ipred(16, ssse3);
        init_all_ipred(32, ssse3);
    }

    if (EXTERNAL_AVX(cpu_flags)) {
        dsp->itxfm_add[TX_8X8][DCT_DCT] = ff_vp9_idct_idct_8x8_add_avx;
        dsp->itxfm_add[TX_8X8][ADST_DCT]  = ff_vp9_idct_iadst_8x8_add_avx;
        dsp->itxfm_add[TX_8X8][DCT_ADST]  = ff_vp9_iadst_idct_8x8_add_avx;
        dsp->itxfm_add[TX_8X8][ADST_ADST] = ff_vp9_iadst_iadst_8x8_add_avx;
        dsp->itxfm_add[TX_16X16][DCT_DCT] = ff_vp9_idct_idct_16x16_add_avx;
        dsp->itxfm_add[TX_16X16][ADST_DCT]  = ff_vp9_idct_iadst_16x16_add_avx;
        dsp->itxfm_add[TX_16X16][DCT_ADST]  = ff_vp9_iadst_idct_16x16_add_avx;
        dsp->itxfm_add[TX_16X16][ADST_ADST] = ff_vp9_iadst_iadst_16x16_add_avx;
        dsp->itxfm_add[TX_32X32][ADST_ADST] =
        dsp->itxfm_add[TX_32X32][ADST_DCT] =
        dsp->itxfm_add[TX_32X32][DCT_ADST] =
        dsp->itxfm_add[TX_32X32][DCT_DCT] = ff_vp9_idct_idct_32x32_add_avx;
        init_lpf(avx);
        init_dir_tm_h_ipred(8, avx);
        init_dir_tm_h_ipred(16, avx);
        init_dir_tm_h_ipred(32, avx);
    }
    if (EXTERNAL_AVX_FAST(cpu_flags)) {
        init_fpel(1, 0, 32, put, avx);
        init_fpel(0, 0, 64, put, avx);
        init_ipred(32, avx, v, VERT);
    }

    if (EXTERNAL_AVX2(cpu_flags)) {
        init_fpel(1, 1, 32, avg, avx2);
        init_fpel(0, 1, 64, avg, avx2);
        if (ARCH_X86_64) {
#if ARCH_X86_64 && HAVE_AVX2_EXTERNAL
            init_subpel3_32_64(0, put, avx2);
            init_subpel3_32_64(1, avg, avx2);
#endif
        }
        init_dc_ipred(32, avx2);
        init_ipred(32, avx2, h,  HOR);
        init_ipred(32, avx2, tm, TM_VP8);
    }

#undef init_fpel
#undef init_subpel1
#undef init_subpel2
#undef init_subpel3

#endif /
}
