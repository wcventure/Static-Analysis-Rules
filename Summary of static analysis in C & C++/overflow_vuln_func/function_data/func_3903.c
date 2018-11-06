static int read_matrix_params(MLPDecodeContext *m, unsigned int substr, GetBitContext *gbp)
{
    SubStream *s = &m->substream[substr];
    unsigned int mat, ch;
    const int max_primitive_matrices = m->avctx->codec_id == AV_CODEC_ID_MLP
                                     ? MAX_MATRICES_MLP
                                     : MAX_MATRICES_TRUEHD;

    if (m->matrix_changed++ > 1) {
        av_log(m->avctx, AV_LOG_ERROR, "Matrices may change only once per access unit.\n");
        return AVERROR_INVALIDDATA;
    }

    s->num_primitive_matrices = get_bits(gbp, 4);

    if (s->num_primitive_matrices > max_primitive_matrices) {
        av_log(m->avctx, AV_LOG_ERROR,
               "Number of primitive matrices cannot be greater than %d.\n",
               max_primitive_matrices);
        s->num_primitive_matrices = 0;
        return AVERROR_INVALIDDATA;
    }

    for (mat = 0; mat < s->num_primitive_matrices; mat++) {
        int frac_bits, max_chan;
        s->matrix_out_ch[mat] = get_bits(gbp, 4);
        frac_bits             = get_bits(gbp, 4);
        s->lsb_bypass   [mat] = get_bits1(gbp);

        if (s->matrix_out_ch[mat] > s->max_matrix_channel) {
            av_log(m->avctx, AV_LOG_ERROR,
                    "Invalid channel %d specified as output from matrix.\n",
                    s->matrix_out_ch[mat]);
            return AVERROR_INVALIDDATA;
        }
        if (frac_bits > 14) {
            av_log(m->avctx, AV_LOG_ERROR,
                    "Too many fractional bits specified.\n");
            return AVERROR_INVALIDDATA;
        }

        max_chan = s->max_matrix_channel;
        if (!s->noise_type)
            max_chan+=2;

        for (ch = 0; ch <= max_chan; ch++) {
            int coeff_val = 0;
            if (get_bits1(gbp))
                coeff_val = get_sbits(gbp, frac_bits + 2);

            s->matrix_coeff[mat][ch] = coeff_val * (1 << (14 - frac_bits));
        }

        if (s->noise_type)
            s->matrix_noise_shift[mat] = get_bits(gbp, 4);
        else
            s->matrix_noise_shift[mat] = 0;
    }

    return 0;
}
