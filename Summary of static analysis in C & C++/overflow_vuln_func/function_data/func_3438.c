static inline int decode_residual_inter(AVSContext *h) {
    int block;

    /
    int cbp= get_ue_golomb(&h->s.gb);
    if(cbp > 63){
        av_log(h->s.avctx, AV_LOG_ERROR, "illegal inter cbp\n");
        return -1;
    }
    h->cbp = cbp_tab[cbp][1];

    /
    if(h->cbp && !h->qp_fixed)
        h->qp += get_se_golomb(&h->s.gb);
    for(block=0;block<4;block++)
        if(h->cbp & (1<<block))
            decode_residual_block(h,&h->s.gb,inter_2dvlc,0,h->qp,
                                  h->cy + h->luma_scan[block], h->l_stride);
    decode_residual_chroma(h);

    return 0;
}
