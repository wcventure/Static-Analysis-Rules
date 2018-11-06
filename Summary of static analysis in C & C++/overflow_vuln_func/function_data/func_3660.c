static void fill_picture_parameters(const AVCodecContext *avctx, AVDXVAContext *ctx, const HEVCContext *h,
                                    DXVA_PicParams_HEVC *pp)
{
    const HEVCFrame *current_picture = h->ref;
    int i, j, k;

    memset(pp, 0, sizeof(*pp));

    pp->PicWidthInMinCbsY  = h->sps->min_cb_width;
    pp->PicHeightInMinCbsY = h->sps->min_cb_height;

    pp->wFormatAndSequenceInfoFlags = (h->sps->chroma_format_idc          <<  0) |
                                      (h->sps->separate_colour_plane_flag <<  2) |
                                      ((h->sps->bit_depth - 8)            <<  3) |
                                      ((h->sps->bit_depth - 8)            <<  6) |
                                      ((h->sps->log2_max_poc_lsb - 4)     <<  9) |
                                      (0                                  << 13) |
                                      (0                                  << 14) |
                                      (0                                  << 15);

    fill_picture_entry(&pp->CurrPic, ff_dxva2_get_surface_index(avctx, ctx, current_picture->frame), 0);

    pp->sps_max_dec_pic_buffering_minus1         = h->sps->temporal_layer[h->sps->max_sub_layers - 1].max_dec_pic_buffering - 1;
    pp->log2_min_luma_coding_block_size_minus3   = h->sps->log2_min_cb_size - 3;
    pp->log2_diff_max_min_luma_coding_block_size = h->sps->log2_diff_max_min_coding_block_size;
    pp->log2_min_transform_block_size_minus2     = h->sps->log2_min_tb_size - 2;
    pp->log2_diff_max_min_transform_block_size   = h->sps->log2_max_trafo_size  - h->sps->log2_min_tb_size;
    pp->max_transform_hierarchy_depth_inter      = h->sps->max_transform_hierarchy_depth_inter;
    pp->max_transform_hierarchy_depth_intra      = h->sps->max_transform_hierarchy_depth_intra;
    pp->num_short_term_ref_pic_sets              = h->sps->nb_st_rps;
    pp->num_long_term_ref_pics_sps               = h->sps->num_long_term_ref_pics_sps;

    pp->num_ref_idx_l0_default_active_minus1     = h->pps->num_ref_idx_l0_default_active - 1;
    pp->num_ref_idx_l1_default_active_minus1     = h->pps->num_ref_idx_l1_default_active - 1;
    pp->init_qp_minus26                          = h->pps->pic_init_qp_minus26;

    if (h->sh.short_term_ref_pic_set_sps_flag == 0 && h->sh.short_term_rps) {
        pp->ucNumDeltaPocsOfRefRpsIdx            = h->sh.short_term_rps->num_delta_pocs;
        pp->wNumBitsForShortTermRPSInSlice       = h->sh.short_term_ref_pic_set_size;
    }

    pp->dwCodingParamToolFlags = (h->sps->scaling_list_enable_flag               <<  0) |
                                 (h->sps->amp_enabled_flag                       <<  1) |
                                 (h->sps->sao_enabled                            <<  2) |
                                 (h->sps->pcm_enabled_flag                       <<  3) |
                                 ((h->sps->pcm_enabled_flag ? (h->sps->pcm.bit_depth - 1) : 0)            <<  4) |
                                 ((h->sps->pcm_enabled_flag ? (h->sps->pcm.bit_depth_chroma - 1) : 0)     <<  8) |
                                 ((h->sps->pcm_enabled_flag ? (h->sps->pcm.log2_min_pcm_cb_size - 3) : 0) << 12) |
                                 ((h->sps->pcm_enabled_flag ? (h->sps->pcm.log2_max_pcm_cb_size - h->sps->pcm.log2_min_pcm_cb_size) : 0) << 14) |
                                 (h->sps->pcm.loop_filter_disable_flag           << 16) |
                                 (h->sps->long_term_ref_pics_present_flag        << 17) |
                                 (h->sps->sps_temporal_mvp_enabled_flag          << 18) |
                                 (h->sps->sps_strong_intra_smoothing_enable_flag << 19) |
                                 (h->pps->dependent_slice_segments_enabled_flag  << 20) |
                                 (h->pps->output_flag_present_flag               << 21) |
                                 (h->pps->num_extra_slice_header_bits            << 22) |
                                 (h->pps->sign_data_hiding_flag                  << 25) |
                                 (h->pps->cabac_init_present_flag                << 26) |
                                 (0                                              << 27);

    pp->dwCodingSettingPicturePropertyFlags = (h->pps->constrained_intra_pred_flag                <<  0) |
                                              (h->pps->transform_skip_enabled_flag                <<  1) |
                                              (h->pps->cu_qp_delta_enabled_flag                   <<  2) |
                                              (h->pps->pic_slice_level_chroma_qp_offsets_present_flag <<  3) |
                                              (h->pps->weighted_pred_flag                         <<  4) |
                                              (h->pps->weighted_bipred_flag                       <<  5) |
                                              (h->pps->transquant_bypass_enable_flag              <<  6) |
                                              (h->pps->tiles_enabled_flag                         <<  7) |
                                              (h->pps->entropy_coding_sync_enabled_flag           <<  8) |
                                              (h->pps->uniform_spacing_flag                       <<  9) |
                                              ((h->pps->tiles_enabled_flag ? h->pps->loop_filter_across_tiles_enabled_flag : 0) << 10) |
                                              (h->pps->seq_loop_filter_across_slices_enabled_flag << 11) |
                                              (h->pps->deblocking_filter_override_enabled_flag    << 12) |
                                              (h->pps->disable_dbf                                << 13) |
                                              (h->pps->lists_modification_present_flag            << 14) |
                                              (h->pps->slice_header_extension_present_flag        << 15) |
                                              (IS_IRAP(h)                                         << 16) |
                                              (IS_IDR(h)                                          << 17) |
                                              /
                                              (IS_IRAP(h)                                         << 18) |
                                              (0                                                  << 19);
    pp->pps_cb_qp_offset            = h->pps->cb_qp_offset;
    pp->pps_cr_qp_offset            = h->pps->cr_qp_offset;
    if (h->pps->tiles_enabled_flag) {
        pp->num_tile_columns_minus1 = h->pps->num_tile_columns - 1;
        pp->num_tile_rows_minus1    = h->pps->num_tile_rows - 1;

        if (!h->pps->uniform_spacing_flag) {
            for (i = 0; i < h->pps->num_tile_columns; i++)
                pp->column_width_minus1[i] = h->pps->column_width[i] - 1;

            for (i = 0; i < h->pps->num_tile_rows; i++)
                pp->row_height_minus1[i] = h->pps->row_height[i] - 1;
        }
    }

    pp->diff_cu_qp_delta_depth           = h->pps->diff_cu_qp_delta_depth;
    pp->pps_beta_offset_div2             = h->pps->beta_offset / 2;
    pp->pps_tc_offset_div2               = h->pps->tc_offset / 2;
    pp->log2_parallel_merge_level_minus2 = h->pps->log2_parallel_merge_level - 2;
    pp->CurrPicOrderCntVal               = h->poc;

    // empty the lists
    memset(&pp->RefPicList, 0xff, sizeof(pp->RefPicList));
    memset(&pp->RefPicSetStCurrBefore, 0xff, sizeof(pp->RefPicSetStCurrBefore));
    memset(&pp->RefPicSetStCurrAfter, 0xff, sizeof(pp->RefPicSetStCurrAfter));
    memset(&pp->RefPicSetLtCurr, 0xff, sizeof(pp->RefPicSetLtCurr));

    // fill RefPicList from the DPB
    for (i = 0, j = 0; i < FF_ARRAY_ELEMS(h->DPB); i++) {
        const HEVCFrame *frame = &h->DPB[i];
        if (frame != current_picture && (frame->flags & (HEVC_FRAME_FLAG_LONG_REF | HEVC_FRAME_FLAG_SHORT_REF))) {
            fill_picture_entry(&pp->RefPicList[j], ff_dxva2_get_surface_index(avctx, ctx, frame->frame), !!(frame->flags & HEVC_FRAME_FLAG_LONG_REF));
            pp->PicOrderCntValList[j] = frame->poc;
            j++;
        }
    }

    #define DO_REF_LIST(ref_idx, ref_list) { \
        const RefPicList *rpl = &h->rps[ref_idx]; \
        av_assert0(rpl->nb_refs <= FF_ARRAY_ELEMS(pp->ref_list)); \
        for (j = 0, k = 0; j < rpl->nb_refs; j++) { \
            if (rpl->ref[j]) { \
                pp->ref_list[k] = get_refpic_index(pp, ff_dxva2_get_surface_index(avctx, ctx, rpl->ref[j]->frame)); \
                k++; \
            } \
        } \
    }

    // Fill short term and long term lists
    DO_REF_LIST(ST_CURR_BEF, RefPicSetStCurrBefore);
    DO_REF_LIST(ST_CURR_AFT, RefPicSetStCurrAfter);
    DO_REF_LIST(LT_CURR, RefPicSetLtCurr);

    pp->StatusReportFeedbackNumber = 1 + DXVA_CONTEXT_REPORT_ID(avctx, ctx)++;
}
