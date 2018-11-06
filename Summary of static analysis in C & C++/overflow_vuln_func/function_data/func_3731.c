static int decode_hq_slice(DiracContext *s, DiracSlice *slice, uint8_t *tmp_buf)
{
    int i, level, orientation, quant_idx;
    int qfactor[MAX_DWT_LEVELS][4], qoffset[MAX_DWT_LEVELS][4];
    GetBitContext *gb = &slice->gb;
    SliceCoeffs coeffs_num[MAX_DWT_LEVELS];

    skip_bits_long(gb, 8*s->highquality.prefix_bytes);
    quant_idx = get_bits(gb, 8);

    if (quant_idx > DIRAC_MAX_QUANT_INDEX) {
        av_log(s->avctx, AV_LOG_ERROR, "Invalid quantization index - %i\n", quant_idx);
        return AVERROR_INVALIDDATA;
    }

    /
    for (level = 0; level < s->wavelet_depth; level++) {
        for (orientation = !!level; orientation < 4; orientation++) {
            const int quant = FFMAX(quant_idx - s->lowdelay.quant[level][orientation], 0);
            qfactor[level][orientation] = ff_dirac_qscale_tab[quant];
            qoffset[level][orientation] = ff_dirac_qoffset_intra_tab[quant] + 2;
        }
    }

    /
    for (i = 0; i < 3; i++) {
        int coef_num, coef_par, off = 0;
        int64_t length = s->highquality.size_scaler*get_bits(gb, 8);
        int64_t bits_end = get_bits_count(gb) + 8*length;
        const uint8_t *addr = align_get_bits(gb);

        if (length*8 > get_bits_left(gb)) {
            av_log(s->avctx, AV_LOG_ERROR, "end too far away\n");
            return AVERROR_INVALIDDATA;
        }

        coef_num = subband_coeffs(s, slice->slice_x, slice->slice_y, i, coeffs_num);

        if (s->pshift)
            coef_par = ff_dirac_golomb_read_32bit(s->reader_ctx, addr,
                                                  length, tmp_buf, coef_num);
        else
            coef_par = ff_dirac_golomb_read_16bit(s->reader_ctx, addr,
                                                  length, tmp_buf, coef_num);

        if (coef_num > coef_par) {
            const int start_b = coef_par * (1 << (s->pshift + 1));
            const int end_b   = coef_num * (1 << (s->pshift + 1));
            memset(&tmp_buf[start_b], 0, end_b - start_b);
        }

        for (level = 0; level < s->wavelet_depth; level++) {
            const SliceCoeffs *c = &coeffs_num[level];
            for (orientation = !!level; orientation < 4; orientation++) {
                const SubBand *b1 = &s->plane[i].band[level][orientation];
                uint8_t *buf = b1->ibuf + c->top * b1->stride + (c->left << (s->pshift + 1));

                /
                const int qfunc = s->pshift + 2*(c->tot_h <= 2);
                s->diracdsp.dequant_subband[qfunc](&tmp_buf[off], buf, b1->stride,
                                                   qfactor[level][orientation],
                                                   qoffset[level][orientation],
                                                   c->tot_v, c->tot_h);

                off += c->tot << (s->pshift + 1);
            }
        }

        skip_bits_long(gb, bits_end - get_bits_count(gb));
    }

    return 0;
}
