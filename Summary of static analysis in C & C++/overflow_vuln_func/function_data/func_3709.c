void dsputil_init_mmx(DSPContext* c, AVCodecContext *avctx)
{
    mm_flags = mm_support();

    if (avctx->dsp_mask) {
        if (avctx->dsp_mask & FF_MM_FORCE)
            mm_flags |= (avctx->dsp_mask & 0xffff);
        else
            mm_flags &= ~(avctx->dsp_mask & 0xffff);
    }

#if 0
    av_log(avctx, AV_LOG_INFO, "libavcodec: CPU flags:");
    if (mm_flags & MM_MMX)
        av_log(avctx, AV_LOG_INFO, " mmx");
    if (mm_flags & MM_MMXEXT)
        av_log(avctx, AV_LOG_INFO, " mmxext");
    if (mm_flags & MM_3DNOW)
        av_log(avctx, AV_LOG_INFO, " 3dnow");
    if (mm_flags & MM_SSE)
        av_log(avctx, AV_LOG_INFO, " sse");
    if (mm_flags & MM_SSE2)
        av_log(avctx, AV_LOG_INFO, " sse2");
    av_log(avctx, AV_LOG_INFO, "\n");
#endif

    if (mm_flags & MM_MMX) {
        const int idct_algo= avctx->idct_algo;

#ifdef CONFIG_ENCODERS
        const int dct_algo = avctx->dct_algo;
        if(dct_algo==FF_DCT_AUTO || dct_algo==FF_DCT_MMX){
            if(mm_flags & MM_SSE2){
                c->fdct = ff_fdct_sse2;
            }else if(mm_flags & MM_MMXEXT){
                c->fdct = ff_fdct_mmx2;
            }else{
                c->fdct = ff_fdct_mmx;
            }
        }
#endif //CONFIG_ENCODERS
        if(avctx->lowres==0){
            if(idct_algo==FF_IDCT_AUTO || idct_algo==FF_IDCT_SIMPLEMMX){
                c->idct_put= ff_simple_idct_put_mmx;
                c->idct_add= ff_simple_idct_add_mmx;
                c->idct    = ff_simple_idct_mmx;
                c->idct_permutation_type= FF_SIMPLE_IDCT_PERM;
#ifdef CONFIG_GPL
            }else if(idct_algo==FF_IDCT_LIBMPEG2MMX){
                if(mm_flags & MM_MMXEXT){
                    c->idct_put= ff_libmpeg2mmx2_idct_put;
                    c->idct_add= ff_libmpeg2mmx2_idct_add;
                    c->idct    = ff_mmxext_idct;
                }else{
                    c->idct_put= ff_libmpeg2mmx_idct_put;
                    c->idct_add= ff_libmpeg2mmx_idct_add;
                    c->idct    = ff_mmx_idct;
                }
                c->idct_permutation_type= FF_LIBMPEG2_IDCT_PERM;
#endif
            }else if((ENABLE_VP3_DECODER || ENABLE_VP5_DECODER || ENABLE_VP6_DECODER) &&
                     idct_algo==FF_IDCT_VP3 &&
                     avctx->codec->id!=CODEC_ID_THEORA &&
                     !(avctx->flags & CODEC_FLAG_BITEXACT)){
                if(mm_flags & MM_SSE2){
                    c->idct_put= ff_vp3_idct_put_sse2;
                    c->idct_add= ff_vp3_idct_add_sse2;
                    c->idct    = ff_vp3_idct_sse2;
                    c->idct_permutation_type= FF_TRANSPOSE_IDCT_PERM;
                }else{
                    ff_vp3_dsp_init_mmx();
                    c->idct_put= ff_vp3_idct_put_mmx;
                    c->idct_add= ff_vp3_idct_add_mmx;
                    c->idct    = ff_vp3_idct_mmx;
                    c->idct_permutation_type= FF_PARTTRANS_IDCT_PERM;
                }
            }else if(idct_algo==FF_IDCT_CAVS){
                    c->idct_permutation_type= FF_TRANSPOSE_IDCT_PERM;
            }else if(idct_algo==FF_IDCT_XVIDMMX){
                if(mm_flags & MM_MMXEXT){
                    c->idct_put= ff_idct_xvid_mmx2_put;
                    c->idct_add= ff_idct_xvid_mmx2_add;
                    c->idct    = ff_idct_xvid_mmx2;
                }else{
                    c->idct_put= ff_idct_xvid_mmx_put;
                    c->idct_add= ff_idct_xvid_mmx_add;
                    c->idct    = ff_idct_xvid_mmx;
                }
            }
        }

#ifdef CONFIG_ENCODERS
        c->get_pixels = get_pixels_mmx;
        c->diff_pixels = diff_pixels_mmx;
#endif //CONFIG_ENCODERS
        c->put_pixels_clamped = put_pixels_clamped_mmx;
        c->put_signed_pixels_clamped = put_signed_pixels_clamped_mmx;
        c->add_pixels_clamped = add_pixels_clamped_mmx;
        c->clear_blocks = clear_blocks_mmx;
#ifdef CONFIG_ENCODERS
        c->pix_sum = pix_sum16_mmx;
#endif //CONFIG_ENCODERS

        c->put_pixels_tab[0][0] = put_pixels16_mmx;
        c->put_pixels_tab[0][1] = put_pixels16_x2_mmx;
        c->put_pixels_tab[0][2] = put_pixels16_y2_mmx;
        c->put_pixels_tab[0][3] = put_pixels16_xy2_mmx;

        c->put_no_rnd_pixels_tab[0][0] = put_pixels16_mmx;
        c->put_no_rnd_pixels_tab[0][1] = put_no_rnd_pixels16_x2_mmx;
        c->put_no_rnd_pixels_tab[0][2] = put_no_rnd_pixels16_y2_mmx;
        c->put_no_rnd_pixels_tab[0][3] = put_no_rnd_pixels16_xy2_mmx;

        c->avg_pixels_tab[0][0] = avg_pixels16_mmx;
        c->avg_pixels_tab[0][1] = avg_pixels16_x2_mmx;
        c->avg_pixels_tab[0][2] = avg_pixels16_y2_mmx;
        c->avg_pixels_tab[0][3] = avg_pixels16_xy2_mmx;

        c->avg_no_rnd_pixels_tab[0][0] = avg_no_rnd_pixels16_mmx;
        c->avg_no_rnd_pixels_tab[0][1] = avg_no_rnd_pixels16_x2_mmx;
        c->avg_no_rnd_pixels_tab[0][2] = avg_no_rnd_pixels16_y2_mmx;
        c->avg_no_rnd_pixels_tab[0][3] = avg_no_rnd_pixels16_xy2_mmx;

        c->put_pixels_tab[1][0] = put_pixels8_mmx;
        c->put_pixels_tab[1][1] = put_pixels8_x2_mmx;
        c->put_pixels_tab[1][2] = put_pixels8_y2_mmx;
        c->put_pixels_tab[1][3] = put_pixels8_xy2_mmx;

        c->put_no_rnd_pixels_tab[1][0] = put_pixels8_mmx;
        c->put_no_rnd_pixels_tab[1][1] = put_no_rnd_pixels8_x2_mmx;
        c->put_no_rnd_pixels_tab[1][2] = put_no_rnd_pixels8_y2_mmx;
        c->put_no_rnd_pixels_tab[1][3] = put_no_rnd_pixels8_xy2_mmx;

        c->avg_pixels_tab[1][0] = avg_pixels8_mmx;
        c->avg_pixels_tab[1][1] = avg_pixels8_x2_mmx;
        c->avg_pixels_tab[1][2] = avg_pixels8_y2_mmx;
        c->avg_pixels_tab[1][3] = avg_pixels8_xy2_mmx;

        c->avg_no_rnd_pixels_tab[1][0] = avg_no_rnd_pixels8_mmx;
        c->avg_no_rnd_pixels_tab[1][1] = avg_no_rnd_pixels8_x2_mmx;
        c->avg_no_rnd_pixels_tab[1][2] = avg_no_rnd_pixels8_y2_mmx;
        c->avg_no_rnd_pixels_tab[1][3] = avg_no_rnd_pixels8_xy2_mmx;

        c->gmc= gmc_mmx;

        c->add_bytes= add_bytes_mmx;
#ifdef CONFIG_ENCODERS
        c->diff_bytes= diff_bytes_mmx;
        c->sum_abs_dctelem= sum_abs_dctelem_mmx;

        c->hadamard8_diff[0]= hadamard8_diff16_mmx;
        c->hadamard8_diff[1]= hadamard8_diff_mmx;

        c->pix_norm1 = pix_norm1_mmx;
        c->sse[0] = (mm_flags & MM_SSE2) ? sse16_sse2 : sse16_mmx;
          c->sse[1] = sse8_mmx;
        c->vsad[4]= vsad_intra16_mmx;

        c->nsse[0] = nsse16_mmx;
        c->nsse[1] = nsse8_mmx;
        if(!(avctx->flags & CODEC_FLAG_BITEXACT)){
            c->vsad[0] = vsad16_mmx;
        }

        if(!(avctx->flags & CODEC_FLAG_BITEXACT)){
            c->try_8x8basis= try_8x8basis_mmx;
        }
        c->add_8x8basis= add_8x8basis_mmx;

        c->ssd_int8_vs_int16 = ssd_int8_vs_int16_mmx;

#endif //CONFIG_ENCODERS

        if (ENABLE_ANY_H263) {
            c->h263_v_loop_filter= h263_v_loop_filter_mmx;
            c->h263_h_loop_filter= h263_h_loop_filter_mmx;
        }
        c->put_h264_chroma_pixels_tab[0]= put_h264_chroma_mc8_mmx;
        c->put_h264_chroma_pixels_tab[1]= put_h264_chroma_mc4_mmx;

        c->h264_idct_dc_add=
        c->h264_idct_add= ff_h264_idct_add_mmx;
        c->h264_idct8_dc_add=
        c->h264_idct8_add= ff_h264_idct8_add_mmx;

        if (mm_flags & MM_MMXEXT) {
            c->prefetch = prefetch_mmx2;

            c->put_pixels_tab[0][1] = put_pixels16_x2_mmx2;
            c->put_pixels_tab[0][2] = put_pixels16_y2_mmx2;

            c->avg_pixels_tab[0][0] = avg_pixels16_mmx2;
            c->avg_pixels_tab[0][1] = avg_pixels16_x2_mmx2;
            c->avg_pixels_tab[0][2] = avg_pixels16_y2_mmx2;

            c->put_pixels_tab[1][1] = put_pixels8_x2_mmx2;
            c->put_pixels_tab[1][2] = put_pixels8_y2_mmx2;

            c->avg_pixels_tab[1][0] = avg_pixels8_mmx2;
            c->avg_pixels_tab[1][1] = avg_pixels8_x2_mmx2;
            c->avg_pixels_tab[1][2] = avg_pixels8_y2_mmx2;

#ifdef CONFIG_ENCODERS
            c->sum_abs_dctelem= sum_abs_dctelem_mmx2;
            c->hadamard8_diff[0]= hadamard8_diff16_mmx2;
            c->hadamard8_diff[1]= hadamard8_diff_mmx2;
            c->vsad[4]= vsad_intra16_mmx2;
#endif //CONFIG_ENCODERS

            c->h264_idct_dc_add= ff_h264_idct_dc_add_mmx2;
            c->h264_idct8_dc_add= ff_h264_idct8_dc_add_mmx2;

            if(!(avctx->flags & CODEC_FLAG_BITEXACT)){
                c->put_no_rnd_pixels_tab[0][1] = put_no_rnd_pixels16_x2_mmx2;
                c->put_no_rnd_pixels_tab[0][2] = put_no_rnd_pixels16_y2_mmx2;
                c->put_no_rnd_pixels_tab[1][1] = put_no_rnd_pixels8_x2_mmx2;
                c->put_no_rnd_pixels_tab[1][2] = put_no_rnd_pixels8_y2_mmx2;
                c->avg_pixels_tab[0][3] = avg_pixels16_xy2_mmx2;
                c->avg_pixels_tab[1][3] = avg_pixels8_xy2_mmx2;
#ifdef CONFIG_ENCODERS
                c->vsad[0] = vsad16_mmx2;
#endif //CONFIG_ENCODERS
            }

#if 1
            SET_QPEL_FUNC(qpel_pixels_tab[0][ 0], qpel16_mc00_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[0][ 1], qpel16_mc10_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[0][ 2], qpel16_mc20_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[0][ 3], qpel16_mc30_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[0][ 4], qpel16_mc01_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[0][ 5], qpel16_mc11_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[0][ 6], qpel16_mc21_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[0][ 7], qpel16_mc31_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[0][ 8], qpel16_mc02_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[0][ 9], qpel16_mc12_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[0][10], qpel16_mc22_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[0][11], qpel16_mc32_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[0][12], qpel16_mc03_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[0][13], qpel16_mc13_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[0][14], qpel16_mc23_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[0][15], qpel16_mc33_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[1][ 0], qpel8_mc00_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[1][ 1], qpel8_mc10_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[1][ 2], qpel8_mc20_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[1][ 3], qpel8_mc30_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[1][ 4], qpel8_mc01_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[1][ 5], qpel8_mc11_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[1][ 6], qpel8_mc21_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[1][ 7], qpel8_mc31_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[1][ 8], qpel8_mc02_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[1][ 9], qpel8_mc12_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[1][10], qpel8_mc22_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[1][11], qpel8_mc32_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[1][12], qpel8_mc03_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[1][13], qpel8_mc13_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[1][14], qpel8_mc23_mmx2)
            SET_QPEL_FUNC(qpel_pixels_tab[1][15], qpel8_mc33_mmx2)
#endif

//FIXME 3dnow too
#define dspfunc(PFX, IDX, NUM) \
    c->PFX ## _pixels_tab[IDX][ 0] = PFX ## NUM ## _mc00_mmx2; \
    c->PFX ## _pixels_tab[IDX][ 1] = PFX ## NUM ## _mc10_mmx2; \
    c->PFX ## _pixels_tab[IDX][ 2] = PFX ## NUM ## _mc20_mmx2; \
    c->PFX ## _pixels_tab[IDX][ 3] = PFX ## NUM ## _mc30_mmx2; \
    c->PFX ## _pixels_tab[IDX][ 4] = PFX ## NUM ## _mc01_mmx2; \
    c->PFX ## _pixels_tab[IDX][ 5] = PFX ## NUM ## _mc11_mmx2; \
    c->PFX ## _pixels_tab[IDX][ 6] = PFX ## NUM ## _mc21_mmx2; \
    c->PFX ## _pixels_tab[IDX][ 7] = PFX ## NUM ## _mc31_mmx2; \
    c->PFX ## _pixels_tab[IDX][ 8] = PFX ## NUM ## _mc02_mmx2; \
    c->PFX ## _pixels_tab[IDX][ 9] = PFX ## NUM ## _mc12_mmx2; \
    c->PFX ## _pixels_tab[IDX][10] = PFX ## NUM ## _mc22_mmx2; \
    c->PFX ## _pixels_tab[IDX][11] = PFX ## NUM ## _mc32_mmx2; \
    c->PFX ## _pixels_tab[IDX][12] = PFX ## NUM ## _mc03_mmx2; \
    c->PFX ## _pixels_tab[IDX][13] = PFX ## NUM ## _mc13_mmx2; \
    c->PFX ## _pixels_tab[IDX][14] = PFX ## NUM ## _mc23_mmx2; \
    c->PFX ## _pixels_tab[IDX][15] = PFX ## NUM ## _mc33_mmx2

            dspfunc(put_h264_qpel, 0, 16);
            dspfunc(put_h264_qpel, 1, 8);
            dspfunc(put_h264_qpel, 2, 4);
            dspfunc(avg_h264_qpel, 0, 16);
            dspfunc(avg_h264_qpel, 1, 8);
            dspfunc(avg_h264_qpel, 2, 4);

            dspfunc(put_2tap_qpel, 0, 16);
            dspfunc(put_2tap_qpel, 1, 8);
            dspfunc(avg_2tap_qpel, 0, 16);
            dspfunc(avg_2tap_qpel, 1, 8);
#undef dspfunc

            c->avg_h264_chroma_pixels_tab[0]= avg_h264_chroma_mc8_mmx2;
            c->avg_h264_chroma_pixels_tab[1]= avg_h264_chroma_mc4_mmx2;
            c->avg_h264_chroma_pixels_tab[2]= avg_h264_chroma_mc2_mmx2;
            c->put_h264_chroma_pixels_tab[2]= put_h264_chroma_mc2_mmx2;
            c->h264_v_loop_filter_luma= h264_v_loop_filter_luma_mmx2;
            c->h264_h_loop_filter_luma= h264_h_loop_filter_luma_mmx2;
            c->h264_v_loop_filter_chroma= h264_v_loop_filter_chroma_mmx2;
            c->h264_h_loop_filter_chroma= h264_h_loop_filter_chroma_mmx2;
            c->h264_v_loop_filter_chroma_intra= h264_v_loop_filter_chroma_intra_mmx2;
            c->h264_h_loop_filter_chroma_intra= h264_h_loop_filter_chroma_intra_mmx2;
            c->h264_loop_filter_strength= h264_loop_filter_strength_mmx2;

            c->weight_h264_pixels_tab[0]= ff_h264_weight_16x16_mmx2;
            c->weight_h264_pixels_tab[1]= ff_h264_weight_16x8_mmx2;
            c->weight_h264_pixels_tab[2]= ff_h264_weight_8x16_mmx2;
            c->weight_h264_pixels_tab[3]= ff_h264_weight_8x8_mmx2;
            c->weight_h264_pixels_tab[4]= ff_h264_weight_8x4_mmx2;
            c->weight_h264_pixels_tab[5]= ff_h264_weight_4x8_mmx2;
            c->weight_h264_pixels_tab[6]= ff_h264_weight_4x4_mmx2;
            c->weight_h264_pixels_tab[7]= ff_h264_weight_4x2_mmx2;

            c->biweight_h264_pixels_tab[0]= ff_h264_biweight_16x16_mmx2;
            c->biweight_h264_pixels_tab[1]= ff_h264_biweight_16x8_mmx2;
            c->biweight_h264_pixels_tab[2]= ff_h264_biweight_8x16_mmx2;
            c->biweight_h264_pixels_tab[3]= ff_h264_biweight_8x8_mmx2;
            c->biweight_h264_pixels_tab[4]= ff_h264_biweight_8x4_mmx2;
            c->biweight_h264_pixels_tab[5]= ff_h264_biweight_4x8_mmx2;
            c->biweight_h264_pixels_tab[6]= ff_h264_biweight_4x4_mmx2;
            c->biweight_h264_pixels_tab[7]= ff_h264_biweight_4x2_mmx2;

#ifdef CONFIG_CAVS_DECODER
            ff_cavsdsp_init_mmx2(c, avctx);
#endif

#ifdef CONFIG_ENCODERS
            c->sub_hfyu_median_prediction= sub_hfyu_median_prediction_mmx2;
#endif //CONFIG_ENCODERS
        } else if (mm_flags & MM_3DNOW) {
            c->prefetch = prefetch_3dnow;

            c->put_pixels_tab[0][1] = put_pixels16_x2_3dnow;
            c->put_pixels_tab[0][2] = put_pixels16_y2_3dnow;

            c->avg_pixels_tab[0][0] = avg_pixels16_3dnow;
            c->avg_pixels_tab[0][1] = avg_pixels16_x2_3dnow;
            c->avg_pixels_tab[0][2] = avg_pixels16_y2_3dnow;

            c->put_pixels_tab[1][1] = put_pixels8_x2_3dnow;
            c->put_pixels_tab[1][2] = put_pixels8_y2_3dnow;

            c->avg_pixels_tab[1][0] = avg_pixels8_3dnow;
            c->avg_pixels_tab[1][1] = avg_pixels8_x2_3dnow;
            c->avg_pixels_tab[1][2] = avg_pixels8_y2_3dnow;

            if(!(avctx->flags & CODEC_FLAG_BITEXACT)){
                c->put_no_rnd_pixels_tab[0][1] = put_no_rnd_pixels16_x2_3dnow;
                c->put_no_rnd_pixels_tab[0][2] = put_no_rnd_pixels16_y2_3dnow;
                c->put_no_rnd_pixels_tab[1][1] = put_no_rnd_pixels8_x2_3dnow;
                c->put_no_rnd_pixels_tab[1][2] = put_no_rnd_pixels8_y2_3dnow;
                c->avg_pixels_tab[0][3] = avg_pixels16_xy2_3dnow;
                c->avg_pixels_tab[1][3] = avg_pixels8_xy2_3dnow;
            }

            SET_QPEL_FUNC(qpel_pixels_tab[0][ 0], qpel16_mc00_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[0][ 1], qpel16_mc10_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[0][ 2], qpel16_mc20_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[0][ 3], qpel16_mc30_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[0][ 4], qpel16_mc01_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[0][ 5], qpel16_mc11_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[0][ 6], qpel16_mc21_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[0][ 7], qpel16_mc31_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[0][ 8], qpel16_mc02_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[0][ 9], qpel16_mc12_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[0][10], qpel16_mc22_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[0][11], qpel16_mc32_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[0][12], qpel16_mc03_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[0][13], qpel16_mc13_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[0][14], qpel16_mc23_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[0][15], qpel16_mc33_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[1][ 0], qpel8_mc00_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[1][ 1], qpel8_mc10_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[1][ 2], qpel8_mc20_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[1][ 3], qpel8_mc30_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[1][ 4], qpel8_mc01_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[1][ 5], qpel8_mc11_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[1][ 6], qpel8_mc21_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[1][ 7], qpel8_mc31_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[1][ 8], qpel8_mc02_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[1][ 9], qpel8_mc12_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[1][10], qpel8_mc22_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[1][11], qpel8_mc32_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[1][12], qpel8_mc03_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[1][13], qpel8_mc13_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[1][14], qpel8_mc23_3dnow)
            SET_QPEL_FUNC(qpel_pixels_tab[1][15], qpel8_mc33_3dnow)

#define dspfunc(PFX, IDX, NUM) \
    c->PFX ## _pixels_tab[IDX][ 0] = PFX ## NUM ## _mc00_3dnow; \
    c->PFX ## _pixels_tab[IDX][ 1] = PFX ## NUM ## _mc10_3dnow; \
    c->PFX ## _pixels_tab[IDX][ 2] = PFX ## NUM ## _mc20_3dnow; \
    c->PFX ## _pixels_tab[IDX][ 3] = PFX ## NUM ## _mc30_3dnow; \
    c->PFX ## _pixels_tab[IDX][ 4] = PFX ## NUM ## _mc01_3dnow; \
    c->PFX ## _pixels_tab[IDX][ 5] = PFX ## NUM ## _mc11_3dnow; \
    c->PFX ## _pixels_tab[IDX][ 6] = PFX ## NUM ## _mc21_3dnow; \
    c->PFX ## _pixels_tab[IDX][ 7] = PFX ## NUM ## _mc31_3dnow; \
    c->PFX ## _pixels_tab[IDX][ 8] = PFX ## NUM ## _mc02_3dnow; \
    c->PFX ## _pixels_tab[IDX][ 9] = PFX ## NUM ## _mc12_3dnow; \
    c->PFX ## _pixels_tab[IDX][10] = PFX ## NUM ## _mc22_3dnow; \
    c->PFX ## _pixels_tab[IDX][11] = PFX ## NUM ## _mc32_3dnow; \
    c->PFX ## _pixels_tab[IDX][12] = PFX ## NUM ## _mc03_3dnow; \
    c->PFX ## _pixels_tab[IDX][13] = PFX ## NUM ## _mc13_3dnow; \
    c->PFX ## _pixels_tab[IDX][14] = PFX ## NUM ## _mc23_3dnow; \
    c->PFX ## _pixels_tab[IDX][15] = PFX ## NUM ## _mc33_3dnow

            dspfunc(put_h264_qpel, 0, 16);
            dspfunc(put_h264_qpel, 1, 8);
            dspfunc(put_h264_qpel, 2, 4);
            dspfunc(avg_h264_qpel, 0, 16);
            dspfunc(avg_h264_qpel, 1, 8);
            dspfunc(avg_h264_qpel, 2, 4);

            dspfunc(put_2tap_qpel, 0, 16);
            dspfunc(put_2tap_qpel, 1, 8);
            dspfunc(avg_2tap_qpel, 0, 16);
            dspfunc(avg_2tap_qpel, 1, 8);

            c->avg_h264_chroma_pixels_tab[0]= avg_h264_chroma_mc8_3dnow;
            c->avg_h264_chroma_pixels_tab[1]= avg_h264_chroma_mc4_3dnow;
        }

#ifdef CONFIG_ENCODERS
        if(mm_flags & MM_SSE2){
            c->sum_abs_dctelem= sum_abs_dctelem_sse2;
            c->hadamard8_diff[0]= hadamard8_diff16_sse2;
            c->hadamard8_diff[1]= hadamard8_diff_sse2;
        }

#ifdef HAVE_SSSE3
        if(mm_flags & MM_SSSE3){
            if(!(avctx->flags & CODEC_FLAG_BITEXACT)){
                c->try_8x8basis= try_8x8basis_ssse3;
            }
            c->add_8x8basis= add_8x8basis_ssse3;
            c->sum_abs_dctelem= sum_abs_dctelem_ssse3;
            c->hadamard8_diff[0]= hadamard8_diff16_ssse3;
            c->hadamard8_diff[1]= hadamard8_diff_ssse3;
        }
#endif
#endif

#ifdef CONFIG_SNOW_DECODER
#if 0
        if(mm_flags & MM_SSE2){
            c->horizontal_compose97i = ff_snow_horizontal_compose97i_sse2;
            c->vertical_compose97i = ff_snow_vertical_compose97i_sse2;
            c->inner_add_yblock = ff_snow_inner_add_yblock_sse2;
        }
        else{
            c->horizontal_compose97i = ff_snow_horizontal_compose97i_mmx;
            c->vertical_compose97i = ff_snow_vertical_compose97i_mmx;
            c->inner_add_yblock = ff_snow_inner_add_yblock_mmx;
        }
#endif
#endif

        if(mm_flags & MM_3DNOW){
#ifdef CONFIG_ENCODERS
            if(!(avctx->flags & CODEC_FLAG_BITEXACT)){
                c->try_8x8basis= try_8x8basis_3dnow;
            }
            c->add_8x8basis= add_8x8basis_3dnow;
#endif //CONFIG_ENCODERS
            c->vorbis_inverse_coupling = vorbis_inverse_coupling_3dnow;
            c->vector_fmul = vector_fmul_3dnow;
            if(!(avctx->flags & CODEC_FLAG_BITEXACT))
                c->float_to_int16 = float_to_int16_3dnow;
        }
        if(mm_flags & MM_3DNOWEXT)
            c->vector_fmul_reverse = vector_fmul_reverse_3dnow2;
        if(mm_flags & MM_SSE){
            c->vorbis_inverse_coupling = vorbis_inverse_coupling_sse;
            c->vector_fmul = vector_fmul_sse;
            c->float_to_int16 = float_to_int16_sse;
            c->vector_fmul_reverse = vector_fmul_reverse_sse;
            c->vector_fmul_add_add = vector_fmul_add_add_sse;
        }
        if(mm_flags & MM_3DNOW)
            c->vector_fmul_add_add = vector_fmul_add_add_3dnow; // faster than sse
    }

#ifdef CONFIG_ENCODERS
    dsputil_init_pix_mmx(c, avctx);
#endif //CONFIG_ENCODERS
#if 0
    // for speed testing
    get_pixels = just_return;
    put_pixels_clamped = just_return;
    add_pixels_clamped = just_return;

    pix_abs16x16 = just_return;
    pix_abs16x16_x2 = just_return;
    pix_abs16x16_y2 = just_return;
    pix_abs16x16_xy2 = just_return;

    put_pixels_tab[0] = just_return;
    put_pixels_tab[1] = just_return;
    put_pixels_tab[2] = just_return;
    put_pixels_tab[3] = just_return;

    put_no_rnd_pixels_tab[0] = just_return;
    put_no_rnd_pixels_tab[1] = just_return;
    put_no_rnd_pixels_tab[2] = just_return;
    put_no_rnd_pixels_tab[3] = just_return;

    avg_pixels_tab[0] = just_return;
    avg_pixels_tab[1] = just_return;
    avg_pixels_tab[2] = just_return;
    avg_pixels_tab[3] = just_return;

    avg_no_rnd_pixels_tab[0] = just_return;
    avg_no_rnd_pixels_tab[1] = just_return;
    avg_no_rnd_pixels_tab[2] = just_return;
    avg_no_rnd_pixels_tab[3] = just_return;

    //av_fdct = just_return;
    //ff_idct = just_return;
#endif
}
