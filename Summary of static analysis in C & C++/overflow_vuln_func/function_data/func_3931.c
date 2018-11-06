static int decode_cblk(Jpeg2000DecoderContext *s, Jpeg2000CodingStyle *codsty,
                       Jpeg2000T1Context *t1, Jpeg2000Cblk *cblk,
                       int width, int height, int bandpos)
{
    int passno = cblk->npasses, pass_t = 2, bpno = cblk->nonzerobits - 1, y, clnpass_cnt = 0;
    int bpass_csty_symbol = JPEG2000_CBLK_BYPASS & codsty->cblk_style;
    int vert_causal_ctx_csty_symbol = JPEG2000_CBLK_VSC & codsty->cblk_style;

    for (y = 0; y < height; y++)
        memset(t1->data[y], 0, width * sizeof(**t1->data));

    /
    if (!cblk->length)
        return 0;

    for (y = 0; y < height+2; y++)
        memset(t1->flags[y], 0, (width + 2)*sizeof(**t1->flags));

    cblk->data[cblk->length] = 0xff;
    cblk->data[cblk->length+1] = 0xff;
    ff_mqc_initdec(&t1->mqc, cblk->data);

    while (passno--) {
        if (bpno < 0) {
            av_log(s->avctx, AV_LOG_ERROR, "bpno invalid\n");
            return AVERROR(EINVAL);
        }
        switch(pass_t) {
        case 0:
            decode_sigpass(t1, width, height, bpno + 1, bandpos,
                           bpass_csty_symbol && (clnpass_cnt >= 4), vert_causal_ctx_csty_symbol);
            break;
        case 1:
            decode_refpass(t1, width, height, bpno + 1);
            if (bpass_csty_symbol && clnpass_cnt >= 4)
                ff_mqc_initdec(&t1->mqc, cblk->data);
            break;
        case 2:
            decode_clnpass(s, t1, width, height, bpno + 1, bandpos,
                           codsty->cblk_style & JPEG2000_CBLK_SEGSYM, vert_causal_ctx_csty_symbol);
            clnpass_cnt = clnpass_cnt + 1;
            if (bpass_csty_symbol && clnpass_cnt >= 4)
                ff_mqc_initdec(&t1->mqc, cblk->data);
            break;
        }

        pass_t++;
        if (pass_t == 3) {
            bpno--;
            pass_t = 0;
        }
    }
    return 0;
}
