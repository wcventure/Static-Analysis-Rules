static int set_sps(HEVCContext *s, const HEVCSPS *sps)
{
    int ret;
    int num = 0, den = 0;

    pic_arrays_free(s);
    ret = pic_arrays_init(s, sps);
    if (ret < 0)
        goto fail;

    s->avctx->coded_width         = sps->width;
    s->avctx->coded_height        = sps->height;
    s->avctx->width               = sps->output_width;
    s->avctx->height              = sps->output_height;
    s->avctx->pix_fmt             = sps->pix_fmt;
    s->avctx->sample_aspect_ratio = sps->vui.sar;
    s->avctx->has_b_frames        = sps->temporal_layer[sps->max_sub_layers - 1].num_reorder_pics;

    if (sps->vui.video_signal_type_present_flag)
        s->avctx->color_range = sps->vui.video_full_range_flag ? AVCOL_RANGE_JPEG
                                                               : AVCOL_RANGE_MPEG;
    else
        s->avctx->color_range = AVCOL_RANGE_MPEG;

    if (sps->vui.colour_description_present_flag) {
        s->avctx->color_primaries = sps->vui.colour_primaries;
        s->avctx->color_trc       = sps->vui.transfer_characteristic;
        s->avctx->colorspace      = sps->vui.matrix_coeffs;
    } else {
        s->avctx->color_primaries = AVCOL_PRI_UNSPECIFIED;
        s->avctx->color_trc       = AVCOL_TRC_UNSPECIFIED;
        s->avctx->colorspace      = AVCOL_SPC_UNSPECIFIED;
    }

    ff_hevc_pred_init(&s->hpc,     sps->bit_depth);
    ff_hevc_dsp_init (&s->hevcdsp, sps->bit_depth);
    ff_videodsp_init (&s->vdsp,    sps->bit_depth);

    if (sps->sao_enabled) {
        av_frame_unref(s->tmp_frame);
        ret = ff_get_buffer(s->avctx, s->tmp_frame, AV_GET_BUFFER_FLAG_REF);
        if (ret < 0)
            goto fail;
        s->frame = s->tmp_frame;
    }

    s->sps = sps;
    s->vps = (HEVCVPS*) s->vps_list[s->sps->vps_id]->data;

    if (s->vps->vps_timing_info_present_flag) {
        num = s->vps->vps_num_units_in_tick;
        den = s->vps->vps_time_scale;
    } else if (sps->vui.vui_timing_info_present_flag) {
        num = sps->vui.vui_num_units_in_tick;
        den = sps->vui.vui_time_scale;
    }

    if (num != 0 && den != 0)
        av_reduce(&s->avctx->time_base.num, &s->avctx->time_base.den,
                  num, den, 1 << 30);

    return 0;

fail:
    pic_arrays_free(s);
    s->sps = NULL;
    return ret;
}
