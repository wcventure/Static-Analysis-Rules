static av_always_inline void encode_mb_internal(MpegEncContext *s,
                                                int motion_x, int motion_y,
                                                int mb_block_height,
                                                int mb_block_width,
                                                int mb_block_count)
{
    int16_t weight[12][64];
    int16_t orig[12][64];
    const int mb_x = s->mb_x;
    const int mb_y = s->mb_y;
    int i;
    int skip_dct[12];
    int dct_offset = s->linesize * 8; // default for progressive frames
    int uv_dct_offset = s->uvlinesize * 8;
    uint8_t *ptr_y, *ptr_cb, *ptr_cr;
    int wrap_y, wrap_c;

    for (i = 0; i < mb_block_count; i++)
        skip_dct[i] = s->skipdct;

    if (s->adaptive_quant) {
        const int last_qp = s->qscale;
        const int mb_xy = mb_x + mb_y * s->mb_stride;

        s->lambda = s->lambda_table[mb_xy];
        update_qscale(s);

        if (!(s->mpv_flags & FF_MPV_FLAG_QP_RD)) {
            s->qscale = s->current_picture_ptr->qscale_table[mb_xy];
            s->dquant = s->qscale - last_qp;

            if (s->out_format == FMT_H263) {
                s->dquant = av_clip(s->dquant, -2, 2);

                if (s->codec_id == AV_CODEC_ID_MPEG4) {
                    if (!s->mb_intra) {
                        if (s->pict_type == AV_PICTURE_TYPE_B) {
                            if (s->dquant & 1 || s->mv_dir & MV_DIRECT)
                                s->dquant = 0;
                        }
                        if (s->mv_type == MV_TYPE_8X8)
                            s->dquant = 0;
                    }
                }
            }
        }
        ff_set_qscale(s, last_qp + s->dquant);
    } else if (s->mpv_flags & FF_MPV_FLAG_QP_RD)
        ff_set_qscale(s, s->qscale + s->dquant);

    wrap_y = s->linesize;
    wrap_c = s->uvlinesize;
    ptr_y  = s->new_picture.f.data[0] +
             (mb_y * 16 * wrap_y)              + mb_x * 16;
    ptr_cb = s->new_picture.f.data[1] +
             (mb_y * mb_block_height * wrap_c) + mb_x * mb_block_width;
    ptr_cr = s->new_picture.f.data[2] +
             (mb_y * mb_block_height * wrap_c) + mb_x * mb_block_width;

    if((mb_x*16+16 > s->width || mb_y*16+16 > s->height) && s->codec_id != AV_CODEC_ID_AMV){
        uint8_t *ebuf = s->edge_emu_buffer + 32;
        int cw = (s->width  + s->chroma_x_shift) >> s->chroma_x_shift;
        int ch = (s->height + s->chroma_y_shift) >> s->chroma_y_shift;
        s->vdsp.emulated_edge_mc(ebuf, ptr_y, wrap_y, 16, 16, mb_x * 16,
                                 mb_y * 16, s->width, s->height);
        ptr_y = ebuf;
        s->vdsp.emulated_edge_mc(ebuf + 18 * wrap_y, ptr_cb, wrap_c, mb_block_width,
                                 mb_block_height, mb_x * mb_block_width, mb_y * mb_block_height,
                                 cw, ch);
        ptr_cb = ebuf + 18 * wrap_y;
        s->vdsp.emulated_edge_mc(ebuf + 18 * wrap_y + 16, ptr_cr, wrap_c, mb_block_width,
                                 mb_block_height, mb_x * mb_block_width, mb_y * mb_block_height,
                                 cw, ch);
        ptr_cr = ebuf + 18 * wrap_y + 16;
    }

    if (s->mb_intra) {
        if (s->flags & CODEC_FLAG_INTERLACED_DCT) {
            int progressive_score, interlaced_score;

            s->interlaced_dct = 0;
            progressive_score = s->dsp.ildct_cmp[4](s, ptr_y,
                                                    NULL, wrap_y, 8) +
                                s->dsp.ildct_cmp[4](s, ptr_y + wrap_y * 8,
                                                    NULL, wrap_y, 8) - 400;

            if (progressive_score > 0) {
                interlaced_score = s->dsp.ildct_cmp[4](s, ptr_y,
                                                       NULL, wrap_y * 2, 8) +
                                   s->dsp.ildct_cmp[4](s, ptr_y + wrap_y,
                                                       NULL, wrap_y * 2, 8);
                if (progressive_score > interlaced_score) {
                    s->interlaced_dct = 1;

                    dct_offset = wrap_y;
                    uv_dct_offset = wrap_c;
                    wrap_y <<= 1;
                    if (s->chroma_format == CHROMA_422 ||
                        s->chroma_format == CHROMA_444)
                        wrap_c <<= 1;
                }
            }
        }

        s->dsp.get_pixels(s->block[0], ptr_y                  , wrap_y);
        s->dsp.get_pixels(s->block[1], ptr_y              + 8 , wrap_y);
        s->dsp.get_pixels(s->block[2], ptr_y + dct_offset     , wrap_y);
        s->dsp.get_pixels(s->block[3], ptr_y + dct_offset + 8 , wrap_y);

        if (s->flags & CODEC_FLAG_GRAY) {
            skip_dct[4] = 1;
            skip_dct[5] = 1;
        } else {
            s->dsp.get_pixels(s->block[4], ptr_cb, wrap_c);
            s->dsp.get_pixels(s->block[5], ptr_cr, wrap_c);
            if (!s->chroma_y_shift && s->chroma_x_shift) { /
                s->dsp.get_pixels(s->block[6], ptr_cb + uv_dct_offset, wrap_c);
                s->dsp.get_pixels(s->block[7], ptr_cr + uv_dct_offset, wrap_c);
            } else if (!s->chroma_y_shift && !s->chroma_x_shift) { /
                s->dsp.get_pixels(s->block[6], ptr_cb + 8, wrap_c);
                s->dsp.get_pixels(s->block[7], ptr_cr + 8, wrap_c);
                s->dsp.get_pixels(s->block[8], ptr_cb + uv_dct_offset, wrap_c);
                s->dsp.get_pixels(s->block[9], ptr_cr + uv_dct_offset, wrap_c);
                s->dsp.get_pixels(s->block[10], ptr_cb + uv_dct_offset + 8, wrap_c);
                s->dsp.get_pixels(s->block[11], ptr_cr + uv_dct_offset + 8, wrap_c);
            }
        }
    } else {
        op_pixels_func (*op_pix)[4];
        qpel_mc_func (*op_qpix)[16];
        uint8_t *dest_y, *dest_cb, *dest_cr;

        dest_y  = s->dest[0];
        dest_cb = s->dest[1];
        dest_cr = s->dest[2];

        if ((!s->no_rounding) || s->pict_type == AV_PICTURE_TYPE_B) {
            op_pix  = s->hdsp.put_pixels_tab;
            op_qpix = s->dsp.put_qpel_pixels_tab;
        } else {
            op_pix  = s->hdsp.put_no_rnd_pixels_tab;
            op_qpix = s->dsp.put_no_rnd_qpel_pixels_tab;
        }

        if (s->mv_dir & MV_DIR_FORWARD) {
            ff_MPV_motion(s, dest_y, dest_cb, dest_cr, 0,
                          s->last_picture.f.data,
                          op_pix, op_qpix);
            op_pix  = s->hdsp.avg_pixels_tab;
            op_qpix = s->dsp.avg_qpel_pixels_tab;
        }
        if (s->mv_dir & MV_DIR_BACKWARD) {
            ff_MPV_motion(s, dest_y, dest_cb, dest_cr, 1,
                          s->next_picture.f.data,
                          op_pix, op_qpix);
        }

        if (s->flags & CODEC_FLAG_INTERLACED_DCT) {
            int progressive_score, interlaced_score;

            s->interlaced_dct = 0;
            progressive_score = s->dsp.ildct_cmp[0](s, dest_y,
                                                    ptr_y,              wrap_y,
                                                    8) +
                                s->dsp.ildct_cmp[0](s, dest_y + wrap_y * 8,
                                                    ptr_y + wrap_y * 8, wrap_y,
                                                    8) - 400;

            if (s->avctx->ildct_cmp == FF_CMP_VSSE)
                progressive_score -= 400;

            if (progressive_score > 0) {
                interlaced_score = s->dsp.ildct_cmp[0](s, dest_y,
                                                       ptr_y,
                                                       wrap_y * 2, 8) +
                                   s->dsp.ildct_cmp[0](s, dest_y + wrap_y,
                                                       ptr_y + wrap_y,
                                                       wrap_y * 2, 8);

                if (progressive_score > interlaced_score) {
                    s->interlaced_dct = 1;

                    dct_offset = wrap_y;
                    uv_dct_offset = wrap_c;
                    wrap_y <<= 1;
                    if (s->chroma_format == CHROMA_422)
                        wrap_c <<= 1;
                }
            }
        }

        s->dsp.diff_pixels(s->block[0], ptr_y, dest_y, wrap_y);
        s->dsp.diff_pixels(s->block[1], ptr_y + 8, dest_y + 8, wrap_y);
        s->dsp.diff_pixels(s->block[2], ptr_y + dct_offset,
                           dest_y + dct_offset, wrap_y);
        s->dsp.diff_pixels(s->block[3], ptr_y + dct_offset + 8,
                           dest_y + dct_offset + 8, wrap_y);

        if (s->flags & CODEC_FLAG_GRAY) {
            skip_dct[4] = 1;
            skip_dct[5] = 1;
        } else {
            s->dsp.diff_pixels(s->block[4], ptr_cb, dest_cb, wrap_c);
            s->dsp.diff_pixels(s->block[5], ptr_cr, dest_cr, wrap_c);
            if (!s->chroma_y_shift) { /
                s->dsp.diff_pixels(s->block[6], ptr_cb + uv_dct_offset,
                                   dest_cb + uv_dct_offset, wrap_c);
                s->dsp.diff_pixels(s->block[7], ptr_cr + uv_dct_offset,
                                   dest_cr + uv_dct_offset, wrap_c);
            }
        }
        /
        if (s->current_picture.mc_mb_var[s->mb_stride * mb_y + mb_x] <
                2 * s->qscale * s->qscale) {
            // FIXME optimize
            if (s->dsp.sad[1](NULL, ptr_y , dest_y,
                              wrap_y, 8) < 20 * s->qscale)
                skip_dct[0] = 1;
            if (s->dsp.sad[1](NULL, ptr_y + 8,
                              dest_y + 8, wrap_y, 8) < 20 * s->qscale)
                skip_dct[1] = 1;
            if (s->dsp.sad[1](NULL, ptr_y + dct_offset,
                              dest_y + dct_offset, wrap_y, 8) < 20 * s->qscale)
                skip_dct[2] = 1;
            if (s->dsp.sad[1](NULL, ptr_y + dct_offset + 8,
                              dest_y + dct_offset + 8,
                              wrap_y, 8) < 20 * s->qscale)
                skip_dct[3] = 1;
            if (s->dsp.sad[1](NULL, ptr_cb, dest_cb,
                              wrap_c, 8) < 20 * s->qscale)
                skip_dct[4] = 1;
            if (s->dsp.sad[1](NULL, ptr_cr, dest_cr,
                              wrap_c, 8) < 20 * s->qscale)
                skip_dct[5] = 1;
            if (!s->chroma_y_shift) { /
                if (s->dsp.sad[1](NULL, ptr_cb + uv_dct_offset,
                                  dest_cb + uv_dct_offset,
                                  wrap_c, 8) < 20 * s->qscale)
                    skip_dct[6] = 1;
                if (s->dsp.sad[1](NULL, ptr_cr + uv_dct_offset,
                                  dest_cr + uv_dct_offset,
                                  wrap_c, 8) < 20 * s->qscale)
                    skip_dct[7] = 1;
            }
        }
    }

    if (s->quantizer_noise_shaping) {
        if (!skip_dct[0])
            get_visual_weight(weight[0], ptr_y                 , wrap_y);
        if (!skip_dct[1])
            get_visual_weight(weight[1], ptr_y              + 8, wrap_y);
        if (!skip_dct[2])
            get_visual_weight(weight[2], ptr_y + dct_offset    , wrap_y);
        if (!skip_dct[3])
            get_visual_weight(weight[3], ptr_y + dct_offset + 8, wrap_y);
        if (!skip_dct[4])
            get_visual_weight(weight[4], ptr_cb                , wrap_c);
        if (!skip_dct[5])
            get_visual_weight(weight[5], ptr_cr                , wrap_c);
        if (!s->chroma_y_shift) { /
            if (!skip_dct[6])
                get_visual_weight(weight[6], ptr_cb + uv_dct_offset,
                                  wrap_c);
            if (!skip_dct[7])
                get_visual_weight(weight[7], ptr_cr + uv_dct_offset,
                                  wrap_c);
        }
        memcpy(orig[0], s->block[0], sizeof(int16_t) * 64 * mb_block_count);
    }

    /
    av_assert2(s->out_format != FMT_MJPEG || s->qscale == 8);
    {
        for (i = 0; i < mb_block_count; i++) {
            if (!skip_dct[i]) {
                int overflow;
                s->block_last_index[i] = s->dct_quantize(s, s->block[i], i, s->qscale, &overflow);
                // FIXME we could decide to change to quantizer instead of
                // clipping
                // JS: I don't think that would be a good idea it could lower
                //     quality instead of improve it. Just INTRADC clipping
                //     deserves changes in quantizer
                if (overflow)
                    clip_coeffs(s, s->block[i], s->block_last_index[i]);
            } else
                s->block_last_index[i] = -1;
        }
        if (s->quantizer_noise_shaping) {
            for (i = 0; i < mb_block_count; i++) {
                if (!skip_dct[i]) {
                    s->block_last_index[i] =
                        dct_quantize_refine(s, s->block[i], weight[i],
                                            orig[i], i, s->qscale);
                }
            }
        }

        if (s->luma_elim_threshold && !s->mb_intra)
            for (i = 0; i < 4; i++)
                dct_single_coeff_elimination(s, i, s->luma_elim_threshold);
        if (s->chroma_elim_threshold && !s->mb_intra)
            for (i = 4; i < mb_block_count; i++)
                dct_single_coeff_elimination(s, i, s->chroma_elim_threshold);

        if (s->mpv_flags & FF_MPV_FLAG_CBP_RD) {
            for (i = 0; i < mb_block_count; i++) {
                if (s->block_last_index[i] == -1)
                    s->coded_score[i] = INT_MAX / 256;
            }
        }
    }

    if ((s->flags & CODEC_FLAG_GRAY) && s->mb_intra) {
        s->block_last_index[4] =
        s->block_last_index[5] = 0;
        s->block[4][0] =
        s->block[5][0] = (1024 + s->c_dc_scale / 2) / s->c_dc_scale;
        if (!s->chroma_y_shift) { /
            for (i=6; i<12; i++) {
                s->block_last_index[i] = 0;
                s->block[i][0] = s->block[4][0];
            }
        }
    }

    // non c quantize code returns incorrect block_last_index FIXME
    if (s->alternate_scan && s->dct_quantize != ff_dct_quantize_c) {
        for (i = 0; i < mb_block_count; i++) {
            int j;
            if (s->block_last_index[i] > 0) {
                for (j = 63; j > 0; j--) {
                    if (s->block[i][s->intra_scantable.permutated[j]])
                        break;
                }
                s->block_last_index[i] = j;
            }
        }
    }

    /
    switch(s->codec_id){ //FIXME funct ptr could be slightly faster
    case AV_CODEC_ID_MPEG1VIDEO:
    case AV_CODEC_ID_MPEG2VIDEO:
        if (CONFIG_MPEG1VIDEO_ENCODER || CONFIG_MPEG2VIDEO_ENCODER)
            ff_mpeg1_encode_mb(s, s->block, motion_x, motion_y);
        break;
    case AV_CODEC_ID_MPEG4:
        if (CONFIG_MPEG4_ENCODER)
            ff_mpeg4_encode_mb(s, s->block, motion_x, motion_y);
        break;
    case AV_CODEC_ID_MSMPEG4V2:
    case AV_CODEC_ID_MSMPEG4V3:
    case AV_CODEC_ID_WMV1:
        if (CONFIG_MSMPEG4_ENCODER)
            ff_msmpeg4_encode_mb(s, s->block, motion_x, motion_y);
        break;
    case AV_CODEC_ID_WMV2:
        if (CONFIG_WMV2_ENCODER)
            ff_wmv2_encode_mb(s, s->block, motion_x, motion_y);
        break;
    case AV_CODEC_ID_H261:
        if (CONFIG_H261_ENCODER)
            ff_h261_encode_mb(s, s->block, motion_x, motion_y);
        break;
    case AV_CODEC_ID_H263:
    case AV_CODEC_ID_H263P:
    case AV_CODEC_ID_FLV1:
    case AV_CODEC_ID_RV10:
    case AV_CODEC_ID_RV20:
        if (CONFIG_H263_ENCODER)
            ff_h263_encode_mb(s, s->block, motion_x, motion_y);
        break;
    case AV_CODEC_ID_MJPEG:
    case AV_CODEC_ID_AMV:
        if (CONFIG_MJPEG_ENCODER)
            ff_mjpeg_encode_mb(s, s->block);
        break;
    default:
        av_assert1(0);
    }
}
