int ff_hevc_decode_nal_vps(HEVCContext *s)
{
    int i,j;
    GetBitContext *gb = &s->HEVClc.gb;
    int vps_id = 0;
    HEVCVPS *vps;
    AVBufferRef *vps_buf = av_buffer_allocz(sizeof(*vps));

    if (!vps_buf)
        return AVERROR(ENOMEM);
    vps = (HEVCVPS*)vps_buf->data;

    av_log(s->avctx, AV_LOG_DEBUG, "Decoding VPS\n");

    vps_id = get_bits(gb, 4);
    if (vps_id >= MAX_VPS_COUNT) {
        av_log(s->avctx, AV_LOG_ERROR, "VPS id out of range: %d\n", vps_id);
        goto err;
    }

    if (get_bits(gb, 2) != 3) { // vps_reserved_three_2bits
        av_log(s->avctx, AV_LOG_ERROR, "vps_reserved_three_2bits is not three\n");
        goto err;
    }

    vps->vps_max_layers               = get_bits(gb, 6) + 1;
    vps->vps_max_sub_layers           = get_bits(gb, 3) + 1;
    vps->vps_temporal_id_nesting_flag = get_bits1(gb);

    if (get_bits(gb, 16) != 0xffff) { // vps_reserved_ffff_16bits
        av_log(s->avctx, AV_LOG_ERROR, "vps_reserved_ffff_16bits is not 0xffff\n");
        goto err;
    }

    if (vps->vps_max_sub_layers > MAX_SUB_LAYERS) {
        av_log(s->avctx, AV_LOG_ERROR, "vps_max_sub_layers out of range: %d\n",
               vps->vps_max_sub_layers);
        goto err;
    }

    parse_ptl(s, &vps->ptl, vps->vps_max_sub_layers);

    vps->vps_sub_layer_ordering_info_present_flag = get_bits1(gb);

    i = vps->vps_sub_layer_ordering_info_present_flag ? 0 : vps->vps_max_sub_layers - 1;
    for (; i < vps->vps_max_sub_layers; i++) {
        vps->vps_max_dec_pic_buffering[i] = get_ue_golomb_long(gb) + 1;
        vps->vps_num_reorder_pics[i]      = get_ue_golomb_long(gb);
        vps->vps_max_latency_increase[i]  = get_ue_golomb_long(gb) - 1;

        if (vps->vps_max_dec_pic_buffering[i] > MAX_DPB_SIZE) {
            av_log(s->avctx, AV_LOG_ERROR, "vps_max_dec_pic_buffering_minus1 out of range: %d\n",
                   vps->vps_max_dec_pic_buffering[i] - 1);
            goto err;
        }
        if (vps->vps_num_reorder_pics[i] > vps->vps_max_dec_pic_buffering[i] - 1) {
            av_log(s->avctx, AV_LOG_ERROR, "vps_max_num_reorder_pics out of range: %d\n",
                   vps->vps_num_reorder_pics[i]);
            goto err;
        }
    }

    vps->vps_max_layer_id   = get_bits(gb, 6);
    vps->vps_num_layer_sets = get_ue_golomb_long(gb) + 1;
    for (i = 1; i < vps->vps_num_layer_sets; i++)
        for (j = 0; j <= vps->vps_max_layer_id; j++)
            skip_bits(gb, 1);  // layer_id_included_flag[i][j]

    vps->vps_timing_info_present_flag = get_bits1(gb);
    if (vps->vps_timing_info_present_flag) {
        vps->vps_num_units_in_tick               = get_bits_long(gb, 32);
        vps->vps_time_scale                      = get_bits_long(gb, 32);
        vps->vps_poc_proportional_to_timing_flag = get_bits1(gb);
        if (vps->vps_poc_proportional_to_timing_flag)
            vps->vps_num_ticks_poc_diff_one = get_ue_golomb_long(gb) + 1;
        vps->vps_num_hrd_parameters = get_ue_golomb_long(gb);
        for (i = 0; i < vps->vps_num_hrd_parameters; i++) {
            int common_inf_present = 1;

            get_ue_golomb_long(gb); // hrd_layer_set_idx
            if (i)
                common_inf_present = get_bits1(gb);
            decode_hrd(s, common_inf_present, vps->vps_max_sub_layers);
        }
    }
    get_bits1(gb); /

    av_buffer_unref(&s->vps_list[vps_id]);
    s->vps_list[vps_id] = vps_buf;
    return 0;

err:
    av_buffer_unref(&vps_buf);
    return AVERROR_INVALIDDATA;
}
