av_cold void ff_vp9dsp_init_x86(VP9DSPContext *dsp)
{
#if HAVE_YASM
    int cpu_flags = av_get_cpu_flags();

#define init_fpel(idx1, idx2, sz, type, opt) \
    dsp->mc[idx1][FILTER_8TAP_SMOOTH ][idx2][0][0] = \
    dsp->mc[idx1][FILTER_8TAP_REGULAR][idx2][0][0] = \
    dsp->mc[idx1][FILTER_8TAP_SHARP  ][idx2][0][0] = \
    dsp->mc[idx1][FILTER_BILINEAR    ][idx2][0][0] = ff_vp9_##type##sz##_##opt

#define init_subpel1(idx1, idx2, idxh, idxv, sz, dir, type, opt) \
    dsp->mc[idx1][FILTER_8TAP_SMOOTH ][idx2][idxh][idxv] = type##_8tap_smooth_##sz##dir##_##opt; \
    dsp->mc[idx1][FILTER_8TAP_REGULAR][idx2][idxh][idxv] = type##_8tap_regular_##sz##dir##_##opt; \
    dsp->mc[idx1][FILTER_8TAP_SHARP  ][idx2][idxh][idxv] = type##_8tap_sharp_##sz##dir##_##opt

#define init_subpel2_32_64(idx, idxh, idxv, dir, type, opt) \
    init_subpel1(0, idx, idxh, idxv, 64, dir, type, opt); \
    init_subpel1(1, idx, idxh, idxv, 32, dir, type, opt)

#define init_subpel2(idx, idxh, idxv, dir, type, opt) \
    init_subpel2_32_64(idx, idxh, idxv, dir, type, opt); \
    init_subpel1(2, idx, idxh, idxv, 16, dir, type, opt); \
    init_subpel1(3, idx, idxh, idxv,  8, dir, type, opt); \
    init_subpel1(4, idx, idxh, idxv,  4, dir, type, opt)

#define init_subpel3(idx, type, opt) \
    init_subpel2(idx, 1, 1, hv, type, opt); \
    init_subpel2(idx, 0, 1, v, type, opt); \
    init_subpel2(idx, 1, 0, h, type, opt)

#define init_lpf(opt) do { \
    if (ARCH_X86_64) { \
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
    } \
} while (0)

#define init_ipred(tx, sz, opt) do { \
    dsp->intra_pred[tx][HOR_PRED]             = ff_vp9_ipred_h_##sz##x##sz##_##opt; \
    dsp->intra_pred[tx][DIAG_DOWN_LEFT_PRED]  = ff_vp9_ipred_dl_##sz##x##sz##_##opt; \
    dsp->intra_pred[tx][DIAG_DOWN_RIGHT_PRED] = ff_vp9_ipred_dr_##sz##x##sz##_##opt; \
    dsp->intra_pred[tx][HOR_DOWN_PRED]        = ff_vp9_ipred_hd_##sz##x##sz##_##opt; \
    dsp->intra_pred[tx][VERT_LEFT_PRED]       = ff_vp9_ipred_vl_##sz##x##sz##_##opt; \
    dsp->intra_pred[tx][HOR_UP_PRED]          = ff_vp9_ipred_hu_##sz##x##sz##_##opt; \
    if (ARCH_X86_64 || tx != TX_32X32) { \
        dsp->intra_pred[tx][VERT_RIGHT_PRED]      = ff_vp9_ipred_vr_##sz##x##sz##_##opt; \
        dsp->intra_pred[tx][TM_VP8_PRED]          = ff_vp9_ipred_tm_##sz##x##sz##_##opt; \
    } \
} while (0)
#define init_dc_ipred(tx, sz, opt) do { \
    init_ipred(tx, sz, opt); \
    dsp->intra_pred[tx][DC_PRED]              = ff_vp9_ipred_dc_##sz##x##sz##_##opt; \
    dsp->intra_pred[tx][LEFT_DC_PRED]         = ff_vp9_ipred_dc_left_##sz##x##sz##_##opt; \
    dsp->intra_pred[tx][TOP_DC_PRED]          = ff_vp9_ipred_dc_top_##sz##x##sz##_##opt; \
} while (0)

    if (EXTERNAL_MMX(cpu_flags)) {
        init_fpel(4, 0,  4, put, mmx);
        init_fpel(3, 0,  8, put, mmx);
        dsp->itxfm_add[4 /][DCT_DCT] =
        dsp->itxfm_add[4 /][ADST_DCT] =
        dsp->itxfm_add[4 /][DCT_ADST] =
        dsp->itxfm_add[4 /][ADST_ADST] = ff_vp9_iwht_iwht_4x4_add_mmx;
        dsp->intra_pred[TX_8X8][VERT_PRED] = ff_vp9_ipred_v_8x8_mmx;
    }

    if (EXTERNAL_MMXEXT(cpu_flags)) {
        init_fpel(4, 1,  4, avg, mmxext);
        init_fpel(3, 1,  8, avg, mmxext);
        dsp->itxfm_add[TX_4X4][DCT_DCT] = ff_vp9_idct_idct_4x4_add_mmxext;
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
        dsp->intra_pred[TX_16X16][VERT_PRED] = ff_vp9_ipred_v_16x16_sse2;
        dsp->intra_pred[TX_32X32][VERT_PRED] = ff_vp9_ipred_v_32x32_sse2;
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
        init_dc_ipred(TX_4X4,    4, ssse3);
        init_dc_ipred(TX_8X8,    8, ssse3);
        init_dc_ipred(TX_16X16, 16, ssse3);
        init_dc_ipred(TX_32X32, 32, ssse3);
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
        init_fpel(1, 0, 32, put, avx);
        init_fpel(0, 0, 64, put, avx);
        init_lpf(avx);
        init_ipred(TX_8X8,    8, avx);
        init_ipred(TX_16X16, 16, avx);
        init_ipred(TX_32X32, 32, avx);
    }

    if (EXTERNAL_AVX2(cpu_flags)) {
        init_fpel(1, 1, 32, avg, avx2);
        init_fpel(0, 1, 64, avg, avx2);
        if (ARCH_X86_64) {
#if ARCH_X86_64 && HAVE_AVX2_EXTERNAL
            init_subpel2_32_64(0, 1, 1, hv, put, avx2);
            init_subpel2_32_64(0, 0, 1, v,  put, avx2);
            init_subpel2_32_64(0, 1, 0, h,  put, avx2);
            init_subpel2_32_64(1, 1, 1, hv, avg, avx2);
            init_subpel2_32_64(1, 0, 1, v,  avg, avx2);
            init_subpel2_32_64(1, 1, 0, h,  avg, avx2);
#endif
        }
        dsp->intra_pred[TX_32X32][DC_PRED] = ff_vp9_ipred_dc_32x32_avx2;
        dsp->intra_pred[TX_32X32][LEFT_DC_PRED] = ff_vp9_ipred_dc_left_32x32_avx2;
        dsp->intra_pred[TX_32X32][TOP_DC_PRED] = ff_vp9_ipred_dc_top_32x32_avx2;
        dsp->intra_pred[TX_32X32][VERT_PRED] = ff_vp9_ipred_v_32x32_avx2;
        dsp->intra_pred[TX_32X32][HOR_PRED] = ff_vp9_ipred_h_32x32_avx2;
        dsp->intra_pred[TX_32X32][TM_VP8_PRED] = ff_vp9_ipred_tm_32x32_avx2;
    }

#undef init_fpel
#undef init_subpel1
#undef init_subpel2
#undef init_subpel3

#endif /
}
