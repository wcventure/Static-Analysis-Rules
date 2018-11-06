int ff_h264_decode_picture_parameter_set(GetBitContext *gb, AVCodecContext *avctx,
                                         H264ParamSets *ps, int bit_length)
{
    AVBufferRef *pps_buf;
    const SPS *sps;
    unsigned int pps_id = get_ue_golomb(gb);
    PPS *pps;
    int qp_bd_offset;
    int bits_left;
    int ret;

    if (pps_id >= MAX_PPS_COUNT) {
        av_log(avctx, AV_LOG_ERROR, "pps_id %u out of range\n", pps_id);
        return AVERROR_INVALIDDATA;
    }

    pps_buf = av_buffer_allocz(sizeof(*pps));
    if (!pps_buf)
        return AVERROR(ENOMEM);
    pps = (PPS*)pps_buf->data;

    pps->data_size = gb->buffer_end - gb->buffer;
    if (pps->data_size > sizeof(pps->data)) {
        av_log(avctx, AV_LOG_WARNING, "Truncating likely oversized PPS "
               "(%"SIZE_SPECIFIER" > %"SIZE_SPECIFIER")\n",
               pps->data_size, sizeof(pps->data));
        pps->data_size = sizeof(pps->data);
    }
    memcpy(pps->data, gb->buffer, pps->data_size);

    pps->sps_id = get_ue_golomb_31(gb);
    if ((unsigned)pps->sps_id >= MAX_SPS_COUNT ||
        !ps->sps_list[pps->sps_id]) {
        av_log(avctx, AV_LOG_ERROR, "sps_id %u out of range\n", pps->sps_id);
        ret = AVERROR_INVALIDDATA;
        goto fail;
    }
    sps = (const SPS*)ps->sps_list[pps->sps_id]->data;
    if (sps->bit_depth_luma > 14) {
        av_log(avctx, AV_LOG_ERROR,
               "Invalid luma bit depth=%d\n",
               sps->bit_depth_luma);
        ret = AVERROR_INVALIDDATA;
        goto fail;
    } else if (sps->bit_depth_luma == 11 || sps->bit_depth_luma == 13) {
        av_log(avctx, AV_LOG_ERROR,
               "Unimplemented luma bit depth=%d\n",
               sps->bit_depth_luma);
        ret = AVERROR_PATCHWELCOME;
        goto fail;
    }

    pps->cabac             = get_bits1(gb);
    pps->pic_order_present = get_bits1(gb);
    pps->slice_group_count = get_ue_golomb(gb) + 1;
    if (pps->slice_group_count > 1) {
        pps->mb_slice_group_map_type = get_ue_golomb(gb);
        av_log(avctx, AV_LOG_ERROR, "FMO not supported\n");
    }
    pps->ref_count[0] = get_ue_golomb(gb) + 1;
    pps->ref_count[1] = get_ue_golomb(gb) + 1;
    if (pps->ref_count[0] - 1 > 32 - 1 || pps->ref_count[1] - 1 > 32 - 1) {
        av_log(avctx, AV_LOG_ERROR, "reference overflow (pps)\n");
        ret = AVERROR_INVALIDDATA;
        goto fail;
    }

    qp_bd_offset = 6 * (sps->bit_depth_luma - 8);

    pps->weighted_pred                        = get_bits1(gb);
    pps->weighted_bipred_idc                  = get_bits(gb, 2);
    pps->init_qp                              = get_se_golomb(gb) + 26 + qp_bd_offset;
    pps->init_qs                              = get_se_golomb(gb) + 26 + qp_bd_offset;
    pps->chroma_qp_index_offset[0]            = get_se_golomb(gb);
    if (pps->chroma_qp_index_offset[0] < -12 || pps->chroma_qp_index_offset[0] > 12) {
        ret = AVERROR_INVALIDDATA;
        goto fail;
    }

    pps->deblocking_filter_parameters_present = get_bits1(gb);
    pps->constrained_intra_pred               = get_bits1(gb);
    pps->redundant_pic_cnt_present            = get_bits1(gb);

    pps->transform_8x8_mode = 0;
    memcpy(pps->scaling_matrix4, sps->scaling_matrix4,
           sizeof(pps->scaling_matrix4));
    memcpy(pps->scaling_matrix8, sps->scaling_matrix8,
           sizeof(pps->scaling_matrix8));

    bits_left = bit_length - get_bits_count(gb);
    if (bits_left > 0 && more_rbsp_data_in_pps(sps, avctx)) {
        pps->transform_8x8_mode = get_bits1(gb);
        ret = decode_scaling_matrices(gb, sps, pps, 0,
                                pps->scaling_matrix4, pps->scaling_matrix8);
        if (ret < 0)
            goto fail;
        // second_chroma_qp_index_offset
        pps->chroma_qp_index_offset[1] = get_se_golomb(gb);
        if (pps->chroma_qp_index_offset[1] < -12 || pps->chroma_qp_index_offset[1] > 12) {
            ret = AVERROR_INVALIDDATA;
            goto fail;
        }
    } else {
        pps->chroma_qp_index_offset[1] = pps->chroma_qp_index_offset[0];
    }

    build_qp_table(pps, 0, pps->chroma_qp_index_offset[0],
                   sps->bit_depth_luma);
    build_qp_table(pps, 1, pps->chroma_qp_index_offset[1],
                   sps->bit_depth_luma);

    init_dequant_tables(pps, sps);

    if (pps->chroma_qp_index_offset[0] != pps->chroma_qp_index_offset[1])
        pps->chroma_qp_diff = 1;

    if (avctx->debug & FF_DEBUG_PICT_INFO) {
        av_log(avctx, AV_LOG_DEBUG,
               "pps:%u sps:%u %s slice_groups:%d ref:%u/%u %s qp:%d/%d/%d/%d %s %s %s %s\n",
               pps_id, pps->sps_id,
               pps->cabac ? "CABAC" : "CAVLC",
               pps->slice_group_count,
               pps->ref_count[0], pps->ref_count[1],
               pps->weighted_pred ? "weighted" : "",
               pps->init_qp, pps->init_qs, pps->chroma_qp_index_offset[0], pps->chroma_qp_index_offset[1],
               pps->deblocking_filter_parameters_present ? "LPAR" : "",
               pps->constrained_intra_pred ? "CONSTR" : "",
               pps->redundant_pic_cnt_present ? "REDU" : "",
               pps->transform_8x8_mode ? "8x8DCT" : "");
    }

    remove_pps(ps, pps_id);
    ps->pps_list[pps_id] = pps_buf;

    return 0;

fail:
    av_buffer_unref(&pps_buf);
    return ret;
}
