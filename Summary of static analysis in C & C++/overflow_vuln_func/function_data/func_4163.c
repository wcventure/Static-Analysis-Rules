int ff_h264_decode_seq_parameter_set(H264Context *h)
{
    int profile_idc, level_idc, constraint_set_flags = 0;
    unsigned int sps_id;
    int i, log2_max_frame_num_minus4;
    SPS *sps;

    profile_idc           = get_bits(&h->gb, 8);
    constraint_set_flags |= get_bits1(&h->gb) << 0;   // constraint_set0_flag
    constraint_set_flags |= get_bits1(&h->gb) << 1;   // constraint_set1_flag
    constraint_set_flags |= get_bits1(&h->gb) << 2;   // constraint_set2_flag
    constraint_set_flags |= get_bits1(&h->gb) << 3;   // constraint_set3_flag
    constraint_set_flags |= get_bits1(&h->gb) << 4;   // constraint_set4_flag
    constraint_set_flags |= get_bits1(&h->gb) << 5;   // constraint_set5_flag
    skip_bits(&h->gb, 2);                             // reserved_zero_2bits
    level_idc = get_bits(&h->gb, 8);
    sps_id    = get_ue_golomb_31(&h->gb);

    if (sps_id >= MAX_SPS_COUNT) {
        av_log(h->avctx, AV_LOG_ERROR, "sps_id %u out of range\n", sps_id);
        return AVERROR_INVALIDDATA;
    }
    sps = av_mallocz(sizeof(SPS));
    if (!sps)
        return AVERROR(ENOMEM);

    sps->sps_id               = sps_id;
    sps->time_offset_length   = 24;
    sps->profile_idc          = profile_idc;
    sps->constraint_set_flags = constraint_set_flags;
    sps->level_idc            = level_idc;

    memset(sps->scaling_matrix4, 16, sizeof(sps->scaling_matrix4));
    memset(sps->scaling_matrix8, 16, sizeof(sps->scaling_matrix8));
    sps->scaling_matrix_present = 0;

    if (sps->profile_idc == 100 ||  // High profile
        sps->profile_idc == 110 ||  // High10 profile
        sps->profile_idc == 122 ||  // High422 profile
        sps->profile_idc == 244 ||  // High444 Predictive profile
        sps->profile_idc ==  44 ||  // Cavlc444 profile
        sps->profile_idc ==  83 ||  // Scalable Constrained High profile (SVC)
        sps->profile_idc ==  86 ||  // Scalable High Intra profile (SVC)
        sps->profile_idc == 118 ||  // Stereo High profile (MVC)
        sps->profile_idc == 128 ||  // Multiview High profile (MVC)
        sps->profile_idc == 138 ||  // Multiview Depth High profile (MVCD)
        sps->profile_idc == 144) {  // old High444 profile
        sps->chroma_format_idc = get_ue_golomb_31(&h->gb);
        if (sps->chroma_format_idc > 3) {
            avpriv_request_sample(h->avctx, "chroma_format_idc %u",
                                  sps->chroma_format_idc);
            goto fail;
        } else if (sps->chroma_format_idc == 3) {
            sps->residual_color_transform_flag = get_bits1(&h->gb);
        }
        sps->bit_depth_luma   = get_ue_golomb(&h->gb) + 8;
        sps->bit_depth_chroma = get_ue_golomb(&h->gb) + 8;
        if (sps->bit_depth_chroma != sps->bit_depth_luma) {
            avpriv_request_sample(h->avctx,
                                  "Different chroma and luma bit depth");
            goto fail;
        }
        sps->transform_bypass = get_bits1(&h->gb);
        decode_scaling_matrices(h, sps, NULL, 1,
                                sps->scaling_matrix4, sps->scaling_matrix8);
    } else {
        sps->chroma_format_idc = 1;
        sps->bit_depth_luma    = 8;
        sps->bit_depth_chroma  = 8;
    }

    log2_max_frame_num_minus4 = get_ue_golomb(&h->gb);
    if (log2_max_frame_num_minus4 < MIN_LOG2_MAX_FRAME_NUM - 4 ||
        log2_max_frame_num_minus4 > MAX_LOG2_MAX_FRAME_NUM - 4) {
        av_log(h->avctx, AV_LOG_ERROR,
               "log2_max_frame_num_minus4 out of range (0-12): %d\n",
               log2_max_frame_num_minus4);
        goto fail;
    }
    sps->log2_max_frame_num = log2_max_frame_num_minus4 + 4;

    sps->poc_type = get_ue_golomb_31(&h->gb);

    if (sps->poc_type == 0) { // FIXME #define
        sps->log2_max_poc_lsb = get_ue_golomb(&h->gb) + 4;
    } else if (sps->poc_type == 1) { // FIXME #define
        sps->delta_pic_order_always_zero_flag = get_bits1(&h->gb);
        sps->offset_for_non_ref_pic           = get_se_golomb(&h->gb);
        sps->offset_for_top_to_bottom_field   = get_se_golomb(&h->gb);
        sps->poc_cycle_length                 = get_ue_golomb(&h->gb);

        if ((unsigned)sps->poc_cycle_length >=
            FF_ARRAY_ELEMS(sps->offset_for_ref_frame)) {
            av_log(h->avctx, AV_LOG_ERROR,
                   "poc_cycle_length overflow %d\n", sps->poc_cycle_length);
            goto fail;
        }

        for (i = 0; i < sps->poc_cycle_length; i++)
            sps->offset_for_ref_frame[i] = get_se_golomb(&h->gb);
    } else if (sps->poc_type != 2) {
        av_log(h->avctx, AV_LOG_ERROR, "illegal POC type %d\n", sps->poc_type);
        goto fail;
    }

    sps->ref_frame_count = get_ue_golomb_31(&h->gb);
    if (sps->ref_frame_count > H264_MAX_PICTURE_COUNT - 2 ||
        sps->ref_frame_count >= 32U) {
        av_log(h->avctx, AV_LOG_ERROR,
               "too many reference frames %d\n", sps->ref_frame_count);
        goto fail;
    }
    sps->gaps_in_frame_num_allowed_flag = get_bits1(&h->gb);
    sps->mb_width                       = get_ue_golomb(&h->gb) + 1;
    sps->mb_height                      = get_ue_golomb(&h->gb) + 1;
    if ((unsigned)sps->mb_width  >= INT_MAX / 16 ||
        (unsigned)sps->mb_height >= INT_MAX / 16 ||
        av_image_check_size(16 * sps->mb_width,
                            16 * sps->mb_height, 0, h->avctx)) {
        av_log(h->avctx, AV_LOG_ERROR, "mb_width/height overflow\n");
        goto fail;
    }

    sps->frame_mbs_only_flag = get_bits1(&h->gb);
    if (!sps->frame_mbs_only_flag)
        sps->mb_aff = get_bits1(&h->gb);
    else
        sps->mb_aff = 0;

    sps->direct_8x8_inference_flag = get_bits1(&h->gb);
    if (!sps->frame_mbs_only_flag && !sps->direct_8x8_inference_flag) {
        av_log(h->avctx, AV_LOG_ERROR,
               "This stream was generated by a broken encoder, invalid 8x8 inference\n");
        goto fail;
    }

#ifndef ALLOW_INTERLACE
    if (sps->mb_aff)
        av_log(h->avctx, AV_LOG_ERROR,
               "MBAFF support not included; enable it at compile-time.\n");
#endif
    sps->crop = get_bits1(&h->gb);
    if (sps->crop) {
        int crop_left   = get_ue_golomb(&h->gb);
        int crop_right  = get_ue_golomb(&h->gb);
        int crop_top    = get_ue_golomb(&h->gb);
        int crop_bottom = get_ue_golomb(&h->gb);

        if (h->avctx->flags2 & CODEC_FLAG2_IGNORE_CROP) {
            av_log(h->avctx, AV_LOG_DEBUG, "discarding sps cropping, original "
                                           "values are l:%d r:%d t:%d b:%d\n",
                   crop_left, crop_right, crop_top, crop_bottom);

            sps->crop_left   =
            sps->crop_right  =
            sps->crop_top    =
            sps->crop_bottom = 0;
        } else {
            int vsub   = (sps->chroma_format_idc == 1) ? 1 : 0;
            int hsub   = (sps->chroma_format_idc == 1 ||
                          sps->chroma_format_idc == 2) ? 1 : 0;
            int step_x = 1 << hsub;
            int step_y = (2 - sps->frame_mbs_only_flag) << vsub;

            if (crop_left & (0x1F >> (sps->bit_depth_luma > 8)) &&
                !(h->avctx->flags & CODEC_FLAG_UNALIGNED)) {
                crop_left &= ~(0x1F >> (sps->bit_depth_luma > 8));
                av_log(h->avctx, AV_LOG_WARNING,
                       "Reducing left cropping to %d "
                       "chroma samples to preserve alignment.\n",
                       crop_left);
            }

            sps->crop_left   = crop_left   * step_x;
            sps->crop_right  = crop_right  * step_x;
            sps->crop_top    = crop_top    * step_y;
            sps->crop_bottom = crop_bottom * step_y;
        }
    } else {
        sps->crop_left   =
        sps->crop_right  =
        sps->crop_top    =
        sps->crop_bottom =
        sps->crop        = 0;
    }

    sps->vui_parameters_present_flag = get_bits1(&h->gb);
    if (sps->vui_parameters_present_flag) {
        int ret = decode_vui_parameters(h, sps);
        if (ret < 0 && h->avctx->err_recognition & AV_EF_EXPLODE)
            goto fail;
    }

    if (!sps->sar.den)
        sps->sar.den = 1;

    if (h->avctx->debug & FF_DEBUG_PICT_INFO) {
        static const char csp[4][5] = { "Gray", "420", "422", "444" };
        av_log(h->avctx, AV_LOG_DEBUG,
               "sps:%u profile:%d/%d poc:%d ref:%d %dx%d %s %s crop:%u/%u/%u/%u %s %s %"PRId32"/%"PRId32"\n",
               sps_id, sps->profile_idc, sps->level_idc,
               sps->poc_type,
               sps->ref_frame_count,
               sps->mb_width, sps->mb_height,
               sps->frame_mbs_only_flag ? "FRM" : (sps->mb_aff ? "MB-AFF" : "PIC-AFF"),
               sps->direct_8x8_inference_flag ? "8B8" : "",
               sps->crop_left, sps->crop_right,
               sps->crop_top, sps->crop_bottom,
               sps->vui_parameters_present_flag ? "VUI" : "",
               csp[sps->chroma_format_idc],
               sps->timing_info_present_flag ? sps->num_units_in_tick : 0,
               sps->timing_info_present_flag ? sps->time_scale : 0);
    }
    sps->new = 1;

    av_free(h->sps_buffers[sps_id]);
    h->sps_buffers[sps_id] = sps;
    h->sps                 = *sps;

    return 0;

fail:
    av_free(sps);
    return -1;
}
