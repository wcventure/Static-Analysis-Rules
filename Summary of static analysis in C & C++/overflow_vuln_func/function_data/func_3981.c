static int vdpau_hevc_start_frame(AVCodecContext *avctx,
                                  const uint8_t *buffer, uint32_t size)
{
    HEVCContext *h = avctx->priv_data;
    HEVCFrame *pic = h->ref;
    struct vdpau_picture_context *pic_ctx = pic->hwaccel_picture_private;

    VdpPictureInfoHEVC *info = &pic_ctx->info.hevc;

    const HEVCSPS *sps = h->ps.sps;
    const HEVCPPS *pps = h->ps.pps;
    const SliceHeader *sh = &h->sh;
    const ScalingList *sl = pps->scaling_list_data_present_flag ?
                            &pps->scaling_list : &sps->scaling_list;

    /

    /
    info->chroma_format_idc = sps->chroma_format_idc;
    info->separate_colour_plane_flag = sps->separate_colour_plane_flag;
    info->pic_width_in_luma_samples = sps->width;
    info->pic_height_in_luma_samples = sps->height;
    info->bit_depth_luma_minus8 = sps->bit_depth - 8;
    info->bit_depth_chroma_minus8 = sps->bit_depth - 8;
    info->log2_max_pic_order_cnt_lsb_minus4 = sps->log2_max_poc_lsb - 4;
    /
    info->sps_max_dec_pic_buffering_minus1 = sps->temporal_layer[sps->max_sub_layers - 1].max_dec_pic_buffering - 1;
    info->log2_min_luma_coding_block_size_minus3 = sps->log2_min_cb_size - 3;
    info->log2_diff_max_min_luma_coding_block_size = sps->log2_diff_max_min_coding_block_size;
    info->log2_min_transform_block_size_minus2 = sps->log2_min_tb_size - 2;
    info->log2_diff_max_min_transform_block_size = sps->log2_max_trafo_size - sps->log2_min_tb_size;
    info->max_transform_hierarchy_depth_inter = sps->max_transform_hierarchy_depth_inter;
    info->max_transform_hierarchy_depth_intra = sps->max_transform_hierarchy_depth_intra;
    info->scaling_list_enabled_flag = sps->scaling_list_enable_flag;
    /
    for (size_t i = 0; i < 6; i++) {
        for (size_t j = 0; j < 16; j++) {
            /
            uint8_t pos = 4 * ff_hevc_diag_scan4x4_y[j] + ff_hevc_diag_scan4x4_x[j];
            info->ScalingList4x4[i][j] = sl->sl[0][i][pos];
        }
        for (size_t j = 0; j < 64; j++) {
            uint8_t pos = 8 * ff_hevc_diag_scan8x8_y[j] + ff_hevc_diag_scan8x8_x[j];
            /
            info->ScalingList8x8[i][j] = sl->sl[1][i][pos];
            /
            info->ScalingList16x16[i][j] = sl->sl[2][i][pos];
            if (i < 2) {
                /
                info->ScalingList32x32[i][j] = sl->sl[3][i * 3][pos];
            }
        }
        /
        info->ScalingListDCCoeff16x16[i] = sl->sl_dc[0][i];
        if (i < 2) {
            /
            info->ScalingListDCCoeff32x32[i] = sl->sl_dc[1][i * 3];
        }
    }
    info->amp_enabled_flag = sps->amp_enabled_flag;
    info->sample_adaptive_offset_enabled_flag = sps->sao_enabled;
    info->pcm_enabled_flag = sps->pcm_enabled_flag;
    if (info->pcm_enabled_flag) {
        /
        info->pcm_sample_bit_depth_luma_minus1 = sps->pcm.bit_depth - 1;
        /
        info->pcm_sample_bit_depth_chroma_minus1 = sps->pcm.bit_depth_chroma - 1;
        /
        info->log2_min_pcm_luma_coding_block_size_minus3 = sps->pcm.log2_min_pcm_cb_size - 3;
        /
        info->log2_diff_max_min_pcm_luma_coding_block_size = sps->pcm.log2_max_pcm_cb_size - sps->pcm.log2_min_pcm_cb_size;
        /
        info->pcm_loop_filter_disabled_flag = sps->pcm.loop_filter_disable_flag;
    }
    /
    info->num_short_term_ref_pic_sets = sps->nb_st_rps;
    info->long_term_ref_pics_present_flag = sps->long_term_ref_pics_present_flag;
    /
    info->num_long_term_ref_pics_sps = sps->num_long_term_ref_pics_sps;
    info->sps_temporal_mvp_enabled_flag = sps->sps_temporal_mvp_enabled_flag;
    info->strong_intra_smoothing_enabled_flag = sps->sps_strong_intra_smoothing_enable_flag;

    /
    info->dependent_slice_segments_enabled_flag = pps->dependent_slice_segments_enabled_flag;
    info->output_flag_present_flag = pps->output_flag_present_flag;
    info->num_extra_slice_header_bits = pps->num_extra_slice_header_bits;
    info->sign_data_hiding_enabled_flag = pps->sign_data_hiding_flag;
    info->cabac_init_present_flag = pps->cabac_init_present_flag;
    info->num_ref_idx_l0_default_active_minus1 = pps->num_ref_idx_l0_default_active - 1;
    info->num_ref_idx_l1_default_active_minus1 = pps->num_ref_idx_l1_default_active - 1;
    info->init_qp_minus26 = pps->pic_init_qp_minus26;
    info->constrained_intra_pred_flag = pps->constrained_intra_pred_flag;
    info->transform_skip_enabled_flag = pps->transform_skip_enabled_flag;
    info->cu_qp_delta_enabled_flag = pps->cu_qp_delta_enabled_flag;
    /
    info->diff_cu_qp_delta_depth = pps->diff_cu_qp_delta_depth;
    info->pps_cb_qp_offset = pps->cb_qp_offset;
    info->pps_cr_qp_offset = pps->cr_qp_offset;
    info->pps_slice_chroma_qp_offsets_present_flag = pps->pic_slice_level_chroma_qp_offsets_present_flag;
    info->weighted_pred_flag = pps->weighted_pred_flag;
    info->weighted_bipred_flag = pps->weighted_bipred_flag;
    info->transquant_bypass_enabled_flag = pps->transquant_bypass_enable_flag;
    info->tiles_enabled_flag = pps->tiles_enabled_flag;
    info->entropy_coding_sync_enabled_flag = pps->entropy_coding_sync_enabled_flag;
    if (info->tiles_enabled_flag) {
        /
        info->num_tile_columns_minus1 = pps->num_tile_columns - 1;
        /
        info->num_tile_rows_minus1 = pps->num_tile_rows - 1;
        /
        info->uniform_spacing_flag = pps->uniform_spacing_flag;
        /
        for (ssize_t i = 0; i < pps->num_tile_columns; i++) {
            info->column_width_minus1[i] = pps->column_width[i] - 1;
        }
        /
        for (ssize_t i = 0; i < pps->num_tile_rows; i++) {
            info->row_height_minus1[i] = pps->row_height[i] - 1;
        }
        /
        info->loop_filter_across_tiles_enabled_flag = pps->loop_filter_across_tiles_enabled_flag;
    }
    info->pps_loop_filter_across_slices_enabled_flag = pps->seq_loop_filter_across_slices_enabled_flag;
    info->deblocking_filter_control_present_flag = pps->deblocking_filter_control_present_flag;
    /
    info->deblocking_filter_override_enabled_flag = pps->deblocking_filter_override_enabled_flag;
    /
    info->pps_deblocking_filter_disabled_flag = pps->disable_dbf;
    /
    info->pps_beta_offset_div2 = pps->beta_offset / 2;
    /
    info->pps_tc_offset_div2 = pps->tc_offset / 2;
    info->lists_modification_present_flag = pps->lists_modification_present_flag;
    info->log2_parallel_merge_level_minus2 = pps->log2_parallel_merge_level - 2;
    info->slice_segment_header_extension_present_flag = pps->slice_header_extension_present_flag;

    /
    info->IDRPicFlag = IS_IDR(h);
    /
    info->RAPPicFlag = IS_IRAP(h);
    /
    info->CurrRpsIdx = sps->nb_st_rps;
    if (sh->short_term_ref_pic_set_sps_flag == 1) {
        for (size_t i = 0; i < sps->nb_st_rps; i++) {
            if (sh->short_term_rps == &sps->st_rps[i]) {
                info->CurrRpsIdx = i;
                break;
            }
        }
    }
    /
    info->NumPocTotalCurr = ff_hevc_frame_nb_refs(h);
    if (sh->short_term_ref_pic_set_sps_flag == 0 && sh->short_term_rps) {
        /
        info->NumDeltaPocsOfRefRpsIdx = sh->short_term_rps->rps_idx_num_delta_pocs;
    }
    /
    info->NumShortTermPictureSliceHeaderBits = sh->short_term_ref_pic_set_size;
    /
    info->NumLongTermPictureSliceHeaderBits = sh->long_term_ref_pic_set_size;

    /
    info->CurrPicOrderCntVal = h->poc;

    /
    for (size_t i = 0; i < 16; i++) {
        info->RefPics[i] = VDP_INVALID_HANDLE;
        info->PicOrderCntVal[i] = 0;
        info->IsLongTerm[i] = 0;
    }
    for (size_t i = 0, j = 0; i < FF_ARRAY_ELEMS(h->DPB); i++) {
        const HEVCFrame *frame = &h->DPB[i];
        if (frame != h->ref && (frame->flags & (HEVC_FRAME_FLAG_LONG_REF |
                                                HEVC_FRAME_FLAG_SHORT_REF))) {
            if (j > 16) {
                av_log(avctx, AV_LOG_WARNING,
                     "VDPAU only supports up to 16 references in the DPB. "
                     "This frame may not be decoded correctly.\n");
                break;
            }
            /
            info->RefPics[j] = ff_vdpau_get_surface_id(frame->frame);
            /
            info->PicOrderCntVal[j] = frame->poc;
            /
            // XXX: Setting this caused glitches in the nvidia implementation
            // Always setting it to zero, produces correct results
            //info->IsLongTerm[j] = frame->flags & HEVC_FRAME_FLAG_LONG_REF;
            info->IsLongTerm[j] = 0;
            j++;
        }
    }
    /
    info->NumPocStCurrBefore = h->rps[ST_CURR_BEF].nb_refs;
    if (info->NumPocStCurrBefore > 8) {
        av_log(avctx, AV_LOG_WARNING,
             "VDPAU only supports up to 8 references in StCurrBefore. "
             "This frame may not be decoded correctly.\n");
        info->NumPocStCurrBefore = 8;
    }
    /
    info->NumPocStCurrAfter = h->rps[ST_CURR_AFT].nb_refs;
    if (info->NumPocStCurrAfter > 8) {
        av_log(avctx, AV_LOG_WARNING,
             "VDPAU only supports up to 8 references in StCurrAfter. "
             "This frame may not be decoded correctly.\n");
        info->NumPocStCurrAfter = 8;
    }
    /
    info->NumPocLtCurr = h->rps[LT_CURR].nb_refs;
    if (info->NumPocLtCurr > 8) {
        av_log(avctx, AV_LOG_WARNING,
             "VDPAU only supports up to 8 references in LtCurr. "
             "This frame may not be decoded correctly.\n");
        info->NumPocLtCurr = 8;
    }
    /
    for (ssize_t i = 0, j = 0; i < h->rps[ST_CURR_BEF].nb_refs; i++) {
        HEVCFrame *frame = h->rps[ST_CURR_BEF].ref[i];
        if (frame) {
            uint8_t found = 0;
            uintptr_t id = ff_vdpau_get_surface_id(frame->frame);
            for (size_t k = 0; k < 16; k++) {
                if (id == info->RefPics[k]) {
                    info->RefPicSetStCurrBefore[j] = k;
                    j++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                av_log(avctx, AV_LOG_WARNING, "missing surface: %p\n",
                       (void *)id);
            }
        } else {
            av_log(avctx, AV_LOG_WARNING, "missing STR Before frame: %zd\n", i);
        }
    }
    /
    for (ssize_t i = 0, j = 0; i < h->rps[ST_CURR_AFT].nb_refs; i++) {
        HEVCFrame *frame = h->rps[ST_CURR_AFT].ref[i];
        if (frame) {
            uint8_t found = 0;
            uintptr_t id = ff_vdpau_get_surface_id(frame->frame);
            for (size_t k = 0; k < 16; k++) {
                if (id == info->RefPics[k]) {
                    info->RefPicSetStCurrAfter[j] = k;
                    j++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                av_log(avctx, AV_LOG_WARNING, "missing surface: %p\n",
                       (void *)id);
            }
        } else {
            av_log(avctx, AV_LOG_WARNING, "missing STR After frame: %zd\n", i);
        }
    }
    /
    for (ssize_t i = 0, j = 0; i < h->rps[LT_CURR].nb_refs; i++) {
        HEVCFrame *frame = h->rps[LT_CURR].ref[i];
        if (frame) {
            uint8_t found = 0;
            uintptr_t id = ff_vdpau_get_surface_id(frame->frame);
            for (size_t k = 0; k < 16; k++) {
                if (id == info->RefPics[k]) {
                    info->RefPicSetLtCurr[j] = k;
                    j++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                av_log(avctx, AV_LOG_WARNING, "missing surface: %p\n",
                       (void *)id);
            }
        } else {
            av_log(avctx, AV_LOG_WARNING, "missing LTR frame: %zd\n", i);
        }
    }

    return ff_vdpau_common_start_frame(pic_ctx, buffer, size);
}
