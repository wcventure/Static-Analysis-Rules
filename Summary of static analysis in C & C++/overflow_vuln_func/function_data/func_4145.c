static int decode_mb_i(AVSContext *h, int cbp_code)
{
    GetBitContext *gb = &h->gb;
    unsigned pred_mode_uv;
    int block;
    uint8_t top[18];
    uint8_t *left = NULL;
    uint8_t *d;

    ff_cavs_init_mb(h);

    /
    for (block = 0; block < 4; block++) {
        int nA, nB, predpred;
        int pos = scan3x3[block];

        nA = h->pred_mode_Y[pos - 1];
        nB = h->pred_mode_Y[pos - 3];
        predpred = FFMIN(nA, nB);
        if (predpred == NOT_AVAIL) // if either is not available
            predpred = INTRA_L_LP;
        if (!get_bits1(gb)) {
            int rem_mode = get_bits(gb, 2);
            predpred     = rem_mode + (rem_mode >= predpred);
        }
        h->pred_mode_Y[pos] = predpred;
    }
    pred_mode_uv = get_ue_golomb(gb);
    if (pred_mode_uv > 6) {
        av_log(h->avctx, AV_LOG_ERROR, "illegal intra chroma pred mode\n");
        return AVERROR_INVALIDDATA;
    }
    ff_cavs_modify_mb_i(h, &pred_mode_uv);

    /
    if (h->cur.f->pict_type == AV_PICTURE_TYPE_I)
        cbp_code = get_ue_golomb(gb);
    if (cbp_code > 63U) {
        av_log(h->avctx, AV_LOG_ERROR, "illegal intra cbp\n");
        return AVERROR_INVALIDDATA;
    }
    h->cbp = cbp_tab[cbp_code][0];
    if (h->cbp && !h->qp_fixed)
        h->qp = (h->qp + get_se_golomb(gb)) & 63; //qp_delta

    /
    for (block = 0; block < 4; block++) {
        d = h->cy + h->luma_scan[block];
        ff_cavs_load_intra_pred_luma(h, top, &left, block);
        h->intra_pred_l[h->pred_mode_Y[scan3x3[block]]]
            (d, top, left, h->l_stride);
        if (h->cbp & (1<<block))
            decode_residual_block(h, gb, intra_dec, 1, h->qp, d, h->l_stride);
    }

    /
    ff_cavs_load_intra_pred_chroma(h);
    h->intra_pred_c[pred_mode_uv](h->cu, &h->top_border_u[h->mbx * 10],
                                  h->left_border_u, h->c_stride);
    h->intra_pred_c[pred_mode_uv](h->cv, &h->top_border_v[h->mbx * 10],
                                  h->left_border_v, h->c_stride);

    decode_residual_chroma(h);
    ff_cavs_filter(h, I_8X8);
    set_mv_intra(h);
    return 0;
}
