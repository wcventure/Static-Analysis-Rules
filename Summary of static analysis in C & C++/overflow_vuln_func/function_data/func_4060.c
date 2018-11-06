static inline int decode_residual_inter(AVSContext *h)
{
    int block;

    /
    int cbp = get_ue_golomb(&h->gb);
    if (cbp > 63U) {
        av_log(h->avctx, AV_LOG_ERROR, "illegal inter cbp %d\n", cbp);
        return AVERROR_INVALIDDATA;
    }
    h->cbp = cbp_tab[cbp][1];

    /
    if (h->cbp && !h->qp_fixed)
        h->qp = (h->qp + get_se_golomb(&h->gb)) & 63;
    for (block = 0; block < 4; block++)
        if (h->cbp & (1 << block))
            decode_residual_block(h, &h->gb, inter_dec, 0, h->qp,
                                  h->cy + h->luma_scan[block], h->l_stride);
    decode_residual_chroma(h);

    return 0;
}
