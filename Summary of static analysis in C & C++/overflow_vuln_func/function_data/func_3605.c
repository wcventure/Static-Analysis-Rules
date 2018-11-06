int vc1_parse_frame_header_adv(VC1Context *v, GetBitContext* gb)
{
    int pqindex, lowquant;
    int status;
    int mbmodetab, imvtab, icbptab, twomvbptab, fourmvbptab; /
    int scale, shift, i; /

    v->numref=0;
    v->fcm=0;
    v->field_mode=0;
    v->p_frame_skipped = 0;
    if (v->second_field) {
        v->s.pict_type = (v->fptype & 1) ? AV_PICTURE_TYPE_P : AV_PICTURE_TYPE_I;
        if (v->fptype & 4)
            v->s.pict_type = (v->fptype & 1) ? AV_PICTURE_TYPE_BI : AV_PICTURE_TYPE_B;
        v->s.current_picture_ptr->f.pict_type = v->s.pict_type;
        if (!v->pic_header_flag)
            goto parse_common_info;
    }

    if (v->interlace) {
        v->fcm = decode012(gb);
        if (v->fcm) {
            if (v->fcm == 2)
                v->field_mode = 1;
            else
                v->field_mode = 0;
            if (!v->warn_interlaced++)
                av_log(v->s.avctx, AV_LOG_ERROR,
                       "Interlaced frames/fields support is incomplete\n");
        }
    }

    if (v->field_mode) {
        v->fptype = get_bits(gb, 3);
        v->s.pict_type = (v->fptype & 2) ? AV_PICTURE_TYPE_P : AV_PICTURE_TYPE_I;
        if (v->fptype & 4) // B-picture
            v->s.pict_type = (v->fptype & 2) ? AV_PICTURE_TYPE_BI : AV_PICTURE_TYPE_B;
    } else {
        switch (get_unary(gb, 0, 4)) {
        case 0:
            v->s.pict_type = AV_PICTURE_TYPE_P;
            break;
        case 1:
            v->s.pict_type = AV_PICTURE_TYPE_B;
            break;
        case 2:
            v->s.pict_type = AV_PICTURE_TYPE_I;
            break;
        case 3:
            v->s.pict_type = AV_PICTURE_TYPE_BI;
            break;
        case 4:
            v->s.pict_type = AV_PICTURE_TYPE_P; // skipped pic
            v->p_frame_skipped = 1;
            break;
        }
    }
    if (v->tfcntrflag)
        skip_bits(gb, 8);
    if (v->broadcast) {
        if (!v->interlace || v->psf) {
            v->rptfrm = get_bits(gb, 2);
        } else {
            v->tff = get_bits1(gb);
            v->rff = get_bits1(gb);
        }
    }
    if (v->panscanflag) {
        av_log_missing_feature(v->s.avctx, "Pan-scan", 0);
        //...
    }
    if (v->p_frame_skipped) {
        return 0;
    }
    v->rnd = get_bits1(gb);
    if (v->interlace)
        v->uvsamp = get_bits1(gb);
    if (v->field_mode) {
        if (!v->refdist_flag)
            v->refdist = 0;
        else {
            if ((v->s.pict_type != AV_PICTURE_TYPE_B)
                && (v->s.pict_type != AV_PICTURE_TYPE_BI)) {
                v->refdist = get_bits(gb, 2);
                if (v->refdist == 3)
                    v->refdist += get_unary(gb, 0, 16);
            } else {
                v->bfraction_lut_index = get_vlc2(gb, ff_vc1_bfraction_vlc.table, VC1_BFRACTION_VLC_BITS, 1);
                v->bfraction           = ff_vc1_bfraction_lut[v->bfraction_lut_index];
                v->frfd = (v->bfraction * v->refdist) >> 8;
                v->brfd = v->refdist - v->frfd - 1;
                if (v->brfd < 0)
                    v->brfd = 0;
            }
        }
        goto parse_common_info;
    }
    if (v->finterpflag)
        v->interpfrm = get_bits1(gb);
    if (v->s.pict_type == AV_PICTURE_TYPE_B) {
        v->bfraction_lut_index = get_vlc2(gb, ff_vc1_bfraction_vlc.table, VC1_BFRACTION_VLC_BITS, 1);
        v->bfraction           = ff_vc1_bfraction_lut[v->bfraction_lut_index];
        if (v->bfraction == 0) {
            v->s.pict_type = AV_PICTURE_TYPE_BI; /
        }
    }

    parse_common_info:
    if (v->field_mode)
        v->cur_field_type = !(v->tff ^ v->second_field);
    pqindex = get_bits(gb, 5);
    if (!pqindex)
        return -1;
    v->pqindex = pqindex;
    if (v->quantizer_mode == QUANT_FRAME_IMPLICIT)
        v->pq = ff_vc1_pquant_table[0][pqindex];
    else
        v->pq = ff_vc1_pquant_table[1][pqindex];

    v->pquantizer = 1;
    if (v->quantizer_mode == QUANT_FRAME_IMPLICIT)
        v->pquantizer = pqindex < 9;
    if (v->quantizer_mode == QUANT_NON_UNIFORM)
        v->pquantizer = 0;
    v->pqindex = pqindex;
    if (pqindex < 9)
        v->halfpq = get_bits1(gb);
    else
        v->halfpq = 0;
    if (v->quantizer_mode == QUANT_FRAME_EXPLICIT)
        v->pquantizer = get_bits1(gb);
    if (v->postprocflag)
        v->postproc = get_bits(gb, 2);

    if (v->s.pict_type == AV_PICTURE_TYPE_I || v->s.pict_type == AV_PICTURE_TYPE_P)
        v->use_ic = 0;

    if (v->parse_only)
        return 0;

    switch (v->s.pict_type) {
    case AV_PICTURE_TYPE_I:
    case AV_PICTURE_TYPE_BI:
        if (v->fcm == 1) { //interlace frame picture
            status = bitplane_decoding(v->fieldtx_plane, &v->fieldtx_is_raw, v);
            if (status < 0)
                return -1;
            av_log(v->s.avctx, AV_LOG_DEBUG, "FIELDTX plane encoding: "
                   "Imode: %i, Invert: %i\n", status>>1, status&1);
        }
        status = bitplane_decoding(v->acpred_plane, &v->acpred_is_raw, v);
        if (status < 0)
            return -1;
        av_log(v->s.avctx, AV_LOG_DEBUG, "ACPRED plane encoding: "
               "Imode: %i, Invert: %i\n", status>>1, status&1);
        v->condover = CONDOVER_NONE;
        if (v->overlap && v->pq <= 8) {
            v->condover = decode012(gb);
            if (v->condover == CONDOVER_SELECT) {
                status = bitplane_decoding(v->over_flags_plane, &v->overflg_is_raw, v);
                if (status < 0)
                    return -1;
                av_log(v->s.avctx, AV_LOG_DEBUG, "CONDOVER plane encoding: "
                       "Imode: %i, Invert: %i\n", status>>1, status&1);
            }
        }
        break;
    case AV_PICTURE_TYPE_P:
        if (v->field_mode) {
            av_log(v->s.avctx, AV_LOG_ERROR, "P Fields do not work currently\n");
            return -1;
            v->numref = get_bits1(gb);
            if (!v->numref) {
                v->reffield          = get_bits1(gb);
                v->ref_field_type[0] = v->reffield ^ !v->cur_field_type;
            }
        }
        if (v->extended_mv)
            v->mvrange = get_unary(gb, 0, 3);
        else
            v->mvrange = 0;
        if (v->interlace) {
            if (v->extended_dmv)
                v->dmvrange = get_unary(gb, 0, 3);
            else
                v->dmvrange = 0;
            if (v->fcm == 1) { // interlaced frame picture
                v->fourmvswitch = get_bits1(gb);
                v->intcomp      = get_bits1(gb);
                if (v->intcomp) {
                    v->lumscale = get_bits(gb, 6);
                    v->lumshift = get_bits(gb, 6);
                    INIT_LUT(v->lumscale, v->lumshift, v->luty, v->lutuv);
                }
                status = bitplane_decoding(v->s.mbskip_table, &v->skip_is_raw, v);
                av_log(v->s.avctx, AV_LOG_DEBUG, "SKIPMB plane encoding: "
                       "Imode: %i, Invert: %i\n", status>>1, status&1);
                mbmodetab = get_bits(gb, 2);
                if (v->fourmvswitch)
                    v->mbmode_vlc = &ff_vc1_intfr_4mv_mbmode_vlc[mbmodetab];
                else
                    v->mbmode_vlc = &ff_vc1_intfr_non4mv_mbmode_vlc[mbmodetab];
                imvtab         = get_bits(gb, 2);
                v->imv_vlc     = &ff_vc1_1ref_mvdata_vlc[imvtab];
                // interlaced p-picture cbpcy range is [1, 63]
                icbptab        = get_bits(gb, 3);
                v->cbpcy_vlc   = &ff_vc1_icbpcy_vlc[icbptab];
                twomvbptab     = get_bits(gb, 2);
                v->twomvbp_vlc = &ff_vc1_2mv_block_pattern_vlc[twomvbptab];
                if (v->fourmvswitch) {
                    fourmvbptab     = get_bits(gb, 2);
                    v->fourmvbp_vlc = &ff_vc1_4mv_block_pattern_vlc[fourmvbptab];
                }
            }
        }
        v->k_x = v->mvrange + 9 + (v->mvrange >> 1); //k_x can be 9 10 12 13
        v->k_y = v->mvrange + 8; //k_y can be 8 9 10 11
        v->range_x = 1 << (v->k_x - 1);
        v->range_y = 1 << (v->k_y - 1);

        if (v->pq < 5)
            v->tt_index = 0;
        else if (v->pq < 13)
            v->tt_index = 1;
        else
            v->tt_index = 2;
        if (v->fcm != 1) {
            int mvmode;
            mvmode     = get_unary(gb, 1, 4);
            lowquant   = (v->pq > 12) ? 0 : 1;
            v->mv_mode = ff_vc1_mv_pmode_table[lowquant][mvmode];
            if (v->mv_mode == MV_PMODE_INTENSITY_COMP) {
                int mvmode2;
                mvmode2 = get_unary(gb, 1, 3);
                v->mv_mode2 = ff_vc1_mv_pmode_table2[lowquant][mvmode2];
                if (v->field_mode)
                    v->intcompfield = decode210(gb);
                v->lumscale = get_bits(gb, 6);
                v->lumshift = get_bits(gb, 6);
                INIT_LUT(v->lumscale, v->lumshift, v->luty, v->lutuv);
                if ((v->field_mode) && !v->intcompfield) {
                    v->lumscale2 = get_bits(gb, 6);
                    v->lumshift2 = get_bits(gb, 6);
                    INIT_LUT(v->lumscale2, v->lumshift2, v->luty2, v->lutuv2);
                }
                v->use_ic = 1;
            }
            v->qs_last = v->s.quarter_sample;
            if (v->mv_mode == MV_PMODE_1MV_HPEL || v->mv_mode == MV_PMODE_1MV_HPEL_BILIN)
                v->s.quarter_sample = 0;
            else if (v->mv_mode == MV_PMODE_INTENSITY_COMP) {
                if (v->mv_mode2 == MV_PMODE_1MV_HPEL || v->mv_mode2 == MV_PMODE_1MV_HPEL_BILIN)
                    v->s.quarter_sample = 0;
                else
                    v->s.quarter_sample = 1;
            } else
                v->s.quarter_sample = 1;
            v->s.mspel = !(v->mv_mode == MV_PMODE_1MV_HPEL_BILIN
                           || (v->mv_mode == MV_PMODE_INTENSITY_COMP
                               && v->mv_mode2 == MV_PMODE_1MV_HPEL_BILIN));
        }
        if (v->fcm == 0) { // progressive
            if ((v->mv_mode == MV_PMODE_INTENSITY_COMP &&
                 v->mv_mode2 == MV_PMODE_MIXED_MV)
                || v->mv_mode == MV_PMODE_MIXED_MV) {
                status = bitplane_decoding(v->mv_type_mb_plane, &v->mv_type_is_raw, v);
                if (status < 0)
                    return -1;
                av_log(v->s.avctx, AV_LOG_DEBUG, "MB MV Type plane encoding: "
                       "Imode: %i, Invert: %i\n", status>>1, status&1);
            } else {
                v->mv_type_is_raw = 0;
                memset(v->mv_type_mb_plane, 0, v->s.mb_stride * v->s.mb_height);
            }
            status = bitplane_decoding(v->s.mbskip_table, &v->skip_is_raw, v);
            if (status < 0)
                return -1;
            av_log(v->s.avctx, AV_LOG_DEBUG, "MB Skip plane encoding: "
                   "Imode: %i, Invert: %i\n", status>>1, status&1);

            /
            v->s.mv_table_index = get_bits(gb, 2); //but using ff_vc1_ tables
            v->cbpcy_vlc        = &ff_vc1_cbpcy_p_vlc[get_bits(gb, 2)];
        } else if (v->fcm == 1) { // frame interlaced
            v->qs_last          = v->s.quarter_sample;
            v->s.quarter_sample = 1;
            v->s.mspel          = 1;
        } else {    // field interlaced
            mbmodetab = get_bits(gb, 3);
            imvtab = get_bits(gb, 2 + v->numref);
            if (!v->numref)
                v->imv_vlc = &ff_vc1_1ref_mvdata_vlc[imvtab];
            else
                v->imv_vlc = &ff_vc1_2ref_mvdata_vlc[imvtab];
            icbptab = get_bits(gb, 3);
            v->cbpcy_vlc = &ff_vc1_icbpcy_vlc[icbptab];
            if ((v->mv_mode == MV_PMODE_INTENSITY_COMP &&
                v->mv_mode2 == MV_PMODE_MIXED_MV) || v->mv_mode == MV_PMODE_MIXED_MV) {
                fourmvbptab     = get_bits(gb, 2);
                v->fourmvbp_vlc = &ff_vc1_4mv_block_pattern_vlc[fourmvbptab];
                v->mbmode_vlc = &ff_vc1_if_mmv_mbmode_vlc[mbmodetab];
            } else {
                v->mbmode_vlc = &ff_vc1_if_1mv_mbmode_vlc[mbmodetab];
            }
        }
        if (v->dquant) {
            av_log(v->s.avctx, AV_LOG_DEBUG, "VOP DQuant info\n");
            vop_dquant_decoding(v);
        }

        v->ttfrm = 0; //FIXME Is that so ?
        if (v->vstransform) {
            v->ttmbf = get_bits1(gb);
            if (v->ttmbf) {
                v->ttfrm = ff_vc1_ttfrm_to_tt[get_bits(gb, 2)];
            }
        } else {
            v->ttmbf = 1;
            v->ttfrm = TT_8X8;
        }
        break;
    case AV_PICTURE_TYPE_B:
        // TODO: implement interlaced frame B picture decoding
        if (v->fcm == 1)
            return -1;
        if (v->extended_mv)
            v->mvrange = get_unary(gb, 0, 3);
        else
            v->mvrange = 0;
        v->k_x     = v->mvrange + 9 + (v->mvrange >> 1); //k_x can be 9 10 12 13
        v->k_y     = v->mvrange + 8; //k_y can be 8 9 10 11
        v->range_x = 1 << (v->k_x - 1);
        v->range_y = 1 << (v->k_y - 1);

        if (v->pq < 5)
            v->tt_index = 0;
        else if (v->pq < 13)
            v->tt_index = 1;
        else
            v->tt_index = 2;

        if (v->field_mode) {
            int mvmode;
            av_log(v->s.avctx, AV_LOG_ERROR, "B Fields do not work currently\n");
            return -1;
            if (v->extended_dmv)
                v->dmvrange = get_unary(gb, 0, 3);
            mvmode = get_unary(gb, 1, 3);
            lowquant = (v->pq > 12) ? 0 : 1;
            v->mv_mode          = ff_vc1_mv_pmode_table2[lowquant][mvmode];
            v->qs_last          = v->s.quarter_sample;
            v->s.quarter_sample = (v->mv_mode == MV_PMODE_1MV || v->mv_mode == MV_PMODE_MIXED_MV);
            v->s.mspel          = !(v->mv_mode == MV_PMODE_1MV_HPEL_BILIN || v->mv_mode == MV_PMODE_1MV_HPEL);
            status = bitplane_decoding(v->forward_mb_plane, &v->fmb_is_raw, v);
            if (status < 0)
                return -1;
            av_log(v->s.avctx, AV_LOG_DEBUG, "MB Forward Type plane encoding: "
                   "Imode: %i, Invert: %i\n", status>>1, status&1);
            mbmodetab = get_bits(gb, 3);
            if (v->mv_mode == MV_PMODE_MIXED_MV)
                v->mbmode_vlc = &ff_vc1_if_mmv_mbmode_vlc[mbmodetab];
            else
                v->mbmode_vlc = &ff_vc1_if_1mv_mbmode_vlc[mbmodetab];
            imvtab       = get_bits(gb, 3);
            v->imv_vlc   = &ff_vc1_2ref_mvdata_vlc[imvtab];
            icbptab      = get_bits(gb, 3);
            v->cbpcy_vlc = &ff_vc1_icbpcy_vlc[icbptab];
            if (v->mv_mode == MV_PMODE_MIXED_MV) {
                fourmvbptab     = get_bits(gb, 2);
                v->fourmvbp_vlc = &ff_vc1_4mv_block_pattern_vlc[fourmvbptab];
            }
            v->numref = 1; // interlaced field B pictures are always 2-ref
        } else {
            v->mv_mode          = get_bits1(gb) ? MV_PMODE_1MV : MV_PMODE_1MV_HPEL_BILIN;
            v->qs_last          = v->s.quarter_sample;
            v->s.quarter_sample = (v->mv_mode == MV_PMODE_1MV);
            v->s.mspel          = v->s.quarter_sample;
            status              = bitplane_decoding(v->direct_mb_plane, &v->dmb_is_raw, v);
            if (status < 0)
                return -1;
            av_log(v->s.avctx, AV_LOG_DEBUG, "MB Direct Type plane encoding: "
                   "Imode: %i, Invert: %i\n", status>>1, status&1);
            status = bitplane_decoding(v->s.mbskip_table, &v->skip_is_raw, v);
            if (status < 0)
                return -1;
            av_log(v->s.avctx, AV_LOG_DEBUG, "MB Skip plane encoding: "
                   "Imode: %i, Invert: %i\n", status>>1, status&1);
            v->s.mv_table_index = get_bits(gb, 2);
            v->cbpcy_vlc = &ff_vc1_cbpcy_p_vlc[get_bits(gb, 2)];
        }

        if (v->dquant) {
            av_log(v->s.avctx, AV_LOG_DEBUG, "VOP DQuant info\n");
            vop_dquant_decoding(v);
        }

        v->ttfrm = 0;
        if (v->vstransform) {
            v->ttmbf = get_bits1(gb);
            if (v->ttmbf) {
                v->ttfrm = ff_vc1_ttfrm_to_tt[get_bits(gb, 2)];
            }
        } else {
            v->ttmbf = 1;
            v->ttfrm = TT_8X8;
        }
        break;
    }

    /
    v->c_ac_table_index = decode012(gb);
    if (v->s.pict_type == AV_PICTURE_TYPE_I || v->s.pict_type == AV_PICTURE_TYPE_BI) {
        v->y_ac_table_index = decode012(gb);
    }
    /
    v->s.dc_table_index = get_bits1(gb);
    if ((v->s.pict_type == AV_PICTURE_TYPE_I || v->s.pict_type == AV_PICTURE_TYPE_BI)
        && v->dquant) {
        av_log(v->s.avctx, AV_LOG_DEBUG, "VOP DQuant info\n");
        vop_dquant_decoding(v);
    }

    v->bi_type = 0;
    if (v->s.pict_type == AV_PICTURE_TYPE_BI) {
        v->s.pict_type = AV_PICTURE_TYPE_B;
        v->bi_type = 1;
    }
    return 0;
}
