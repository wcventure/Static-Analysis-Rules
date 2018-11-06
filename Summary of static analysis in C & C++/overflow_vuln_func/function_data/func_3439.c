static int decode_mb_i(AVSContext *h, int cbp_code) {
    GetBitContext *gb = &h->s.gb;
    int block, pred_mode_uv;
    uint8_t top[18];
    uint8_t left[18];
    uint8_t *d;

    init_mb(h);

    /
    for(block=0;block<4;block++) {
        int nA,nB,predpred;
        int pos = scan3x3[block];

        nA = h->pred_mode_Y[pos-1];
        nB = h->pred_mode_Y[pos-3];
        predpred = FFMIN(nA,nB);
        if(predpred == NOT_AVAIL) // if either is not available
            predpred = INTRA_L_LP;
        if(!get_bits1(gb)){
            int rem_mode= get_bits(gb, 2);
            predpred = rem_mode + (rem_mode >= predpred);
        }
        h->pred_mode_Y[pos] = predpred;
    }
    pred_mode_uv = get_ue_golomb(gb);
    if(pred_mode_uv > 6) {
        av_log(h->s.avctx, AV_LOG_ERROR, "illegal intra chroma pred mode\n");
        return -1;
    }

    /
    h->pred_mode_Y[3] =  h->pred_mode_Y[5];
    h->pred_mode_Y[6] =  h->pred_mode_Y[8];
    h->top_pred_Y[h->mbx*2+0] = h->pred_mode_Y[7];
    h->top_pred_Y[h->mbx*2+1] = h->pred_mode_Y[8];

    /
    if(!(h->flags & A_AVAIL)) {
        modify_pred(left_modifier_l, &h->pred_mode_Y[4] );
        modify_pred(left_modifier_l, &h->pred_mode_Y[7] );
        modify_pred(left_modifier_c, &pred_mode_uv );
    }
    if(!(h->flags & B_AVAIL)) {
        modify_pred(top_modifier_l, &h->pred_mode_Y[4] );
        modify_pred(top_modifier_l, &h->pred_mode_Y[5] );
        modify_pred(top_modifier_c, &pred_mode_uv );
    }

    /
    if(h->pic_type == FF_I_TYPE)
        cbp_code = get_ue_golomb(gb);
    if(cbp_code > 63){
        av_log(h->s.avctx, AV_LOG_ERROR, "illegal intra cbp\n");
        return -1;
    }
    h->cbp = cbp_tab[cbp_code][0];
    if(h->cbp && !h->qp_fixed)
        h->qp += get_se_golomb(gb); //qp_delta

    /
    for(block=0;block<4;block++) {
        d = h->cy + h->luma_scan[block];
        load_intra_pred_luma(h, top, left, block);
        h->intra_pred_l[h->pred_mode_Y[scan3x3[block]]]
            (d, top, left, h->l_stride);
        if(h->cbp & (1<<block))
            decode_residual_block(h,gb,intra_2dvlc,1,h->qp,d,h->l_stride);
    }

    /
    /
    h->left_border_u[9] = h->left_border_u[8];
    h->left_border_v[9] = h->left_border_v[8];
    h->top_border_u[h->mbx*10+9] = h->top_border_u[h->mbx*10+8];
    h->top_border_v[h->mbx*10+9] = h->top_border_v[h->mbx*10+8];
    if(h->mbx && h->mby) {
        h->top_border_u[h->mbx*10] = h->left_border_u[0] = h->topleft_border_u;
        h->top_border_v[h->mbx*10] = h->left_border_v[0] = h->topleft_border_v;
    } else {
        h->left_border_u[0] = h->left_border_u[1];
        h->left_border_v[0] = h->left_border_v[1];
        h->top_border_u[h->mbx*10] = h->top_border_u[h->mbx*10+1];
        h->top_border_v[h->mbx*10] = h->top_border_v[h->mbx*10+1];
    }
    h->intra_pred_c[pred_mode_uv](h->cu, &h->top_border_u[h->mbx*10],
                                  h->left_border_u, h->c_stride);
    h->intra_pred_c[pred_mode_uv](h->cv, &h->top_border_v[h->mbx*10],
                                  h->left_border_v, h->c_stride);

    decode_residual_chroma(h);
    filter_mb(h,I_8X8);

    /
    h->mv[MV_FWD_X0] = intra_mv;
    set_mvs(&h->mv[MV_FWD_X0], BLK_16X16);
    h->mv[MV_BWD_X0] = intra_mv;
    set_mvs(&h->mv[MV_BWD_X0], BLK_16X16);
    if(h->pic_type != FF_B_TYPE)
        *h->col_type = I_8X8;

    return 0;
}
