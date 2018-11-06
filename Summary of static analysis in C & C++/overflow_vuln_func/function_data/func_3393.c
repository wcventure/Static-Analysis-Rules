void ff_convert_matrix(DSPContext *dsp, int (*qmat)[64],
                       uint16_t (*qmat16)[2][64],
                       const uint16_t *quant_matrix,
                       int bias, int qmin, int qmax, int intra)
{
    int qscale;
    int shift = 0;

    for (qscale = qmin; qscale <= qmax; qscale++) {
        int i;
        if (dsp->fdct == ff_jpeg_fdct_islow_8 ||
            dsp->fdct == ff_jpeg_fdct_islow_10 ||
            dsp->fdct == ff_faandct) {
            for (i = 0; i < 64; i++) {
                const int j = dsp->idct_permutation[i];
                /

                qmat[qscale][i] = (int)((UINT64_C(1) << QMAT_SHIFT) /
                                        (qscale * quant_matrix[j]));
            }
        } else if (dsp->fdct == ff_fdct_ifast) {
            for (i = 0; i < 64; i++) {
                const int j = dsp->idct_permutation[i];
                /

                qmat[qscale][i] = (int)((UINT64_C(1) << (QMAT_SHIFT + 14)) /
                                        (ff_aanscales[i] * qscale * quant_matrix[j]));
            }
        } else {
            for (i = 0; i < 64; i++) {
                const int j = dsp->idct_permutation[i];
                /
                qmat[qscale][i] = (int)((UINT64_C(1) << QMAT_SHIFT) /
                                        (qscale * quant_matrix[j]));
                //qmat  [qscale][i] = (1 << QMAT_SHIFT_MMX) /
                //                    (qscale * quant_matrix[i]);
                qmat16[qscale][0][i] = (1 << QMAT_SHIFT_MMX) /
                                       (qscale * quant_matrix[j]);

                if (qmat16[qscale][0][i] == 0 ||
                    qmat16[qscale][0][i] == 128 * 256)
                    qmat16[qscale][0][i] = 128 * 256 - 1;
                qmat16[qscale][1][i] =
                    ROUNDED_DIV(bias << (16 - QUANT_BIAS_SHIFT),
                                qmat16[qscale][0][i]);
            }
        }

        for (i = intra; i < 64; i++) {
            int64_t max = 8191;
            if (dsp->fdct == ff_fdct_ifast) {
                max = (8191LL * ff_aanscales[i]) >> 14;
            }
            while (((max * qmat[qscale][i]) >> shift) > INT_MAX) {
                shift++;
            }
        }
    }
    if (shift) {
        av_log(NULL, AV_LOG_INFO,
               "Warning, QMAT_SHIFT is larger than %d, overflows possible\n",
               QMAT_SHIFT - shift);
    }
}
