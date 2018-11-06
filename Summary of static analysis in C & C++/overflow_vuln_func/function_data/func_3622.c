static inline void codeblock(DiracContext *s, SubBand *b,
                             GetBitContext *gb, DiracArith *c,
                             int left, int right, int top, int bottom,
                             int blockcnt_one, int is_arith)
{
    int x, y, zero_block;
    int qoffset, qfactor;
    uint8_t *buf;

    /
    if (!blockcnt_one) {
        if (is_arith)
            zero_block = dirac_get_arith_bit(c, CTX_ZERO_BLOCK);
        else
            zero_block = get_bits1(gb);

        if (zero_block)
            return;
    }

    if (s->codeblock_mode && !(s->old_delta_quant && blockcnt_one)) {
        int quant = b->quant;
        if (is_arith)
            quant += dirac_get_arith_int(c, CTX_DELTA_Q_F, CTX_DELTA_Q_DATA);
        else
            quant += dirac_get_se_golomb(gb);
        if (quant < 0) {
            av_log(s->avctx, AV_LOG_ERROR, "Invalid quant\n");
            return;
        }
        b->quant = quant;
    }

    if (b->quant > (DIRAC_MAX_QUANT_INDEX - 1)) {
        av_log(s->avctx, AV_LOG_ERROR, "Unsupported quant %d\n", b->quant);
        b->quant = 0;
        return;
    }

    qfactor = ff_dirac_qscale_tab[b->quant];
    /
    if (!s->num_refs)
        qoffset = ff_dirac_qoffset_intra_tab[b->quant] + 2;
    else
        qoffset = ff_dirac_qoffset_inter_tab[b->quant] + 2;

    buf = b->ibuf + top * b->stride;
    if (is_arith) {
        for (y = top; y < bottom; y++) {
            for (x = left; x < right; x++) {
                if (b->pshift) {
                    coeff_unpack_arith_10(c, qfactor, qoffset, b, (int32_t*)(buf)+x, x, y);
                } else {
                    coeff_unpack_arith_8(c, qfactor, qoffset, b, (int16_t*)(buf)+x, x, y);
                }
            }
            buf += b->stride;
        }
    } else {
        for (y = top; y < bottom; y++) {
            for (x = left; x < right; x++) {
                int val = coeff_unpack_golomb(gb, qfactor, qoffset);
                if (b->pshift) {
                    AV_WN32(&buf[4*x], val);
                } else {
                    AV_WN16(&buf[2*x], val);
                }
            }
            buf += b->stride;
         }
     }
}
