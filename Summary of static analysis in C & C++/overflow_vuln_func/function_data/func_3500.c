static void ra144_encode_subblock(RA144Context *ractx,
                                  const int16_t *sblock_data,
                                  const int16_t *lpc_coefs, unsigned int rms,
                                  PutBitContext *pb)
{
    float data[BLOCKSIZE] = { 0 }, work[LPC_ORDER + BLOCKSIZE];
    float coefs[LPC_ORDER];
    float zero[BLOCKSIZE], cba[BLOCKSIZE], cb1[BLOCKSIZE], cb2[BLOCKSIZE];
    int16_t cba_vect[BLOCKSIZE];
    int cba_idx, cb1_idx, cb2_idx, gain;
    int i, n, m[3];
    float g[3];
    float error, best_error;

    for (i = 0; i < LPC_ORDER; i++) {
        work[i] = ractx->curr_sblock[BLOCKSIZE + i];
        coefs[i] = lpc_coefs[i] * (1/4096.0);
    }

    /
    ff_celp_lp_synthesis_filterf(work + LPC_ORDER, coefs, data, BLOCKSIZE,
                                 LPC_ORDER);
    for (i = 0; i < BLOCKSIZE; i++) {
        zero[i] = work[LPC_ORDER + i];
        data[i] = sblock_data[i] - zero[i];
    }

    /
    memset(work, 0, LPC_ORDER * sizeof(*work));

    cba_idx = adaptive_cb_search(ractx->adapt_cb, work + LPC_ORDER, coefs,
                                 data);
    if (cba_idx) {
        /
        memcpy(cba, work + LPC_ORDER, sizeof(cba));

        ff_copy_and_dup(cba_vect, ractx->adapt_cb, cba_idx + BLOCKSIZE / 2 - 1);
        m[0] = (ff_irms(cba_vect) * rms) >> 12;
    }
    fixed_cb_search(work + LPC_ORDER, coefs, data, cba_idx, &cb1_idx, &cb2_idx);
    for (i = 0; i < BLOCKSIZE; i++) {
        cb1[i] = ff_cb1_vects[cb1_idx][i];
        cb2[i] = ff_cb2_vects[cb2_idx][i];
    }
    ff_celp_lp_synthesis_filterf(work + LPC_ORDER, coefs, cb1, BLOCKSIZE,
                                 LPC_ORDER);
    memcpy(cb1, work + LPC_ORDER, sizeof(cb1));
    m[1] = (ff_cb1_base[cb1_idx] * rms) >> 8;
    ff_celp_lp_synthesis_filterf(work + LPC_ORDER, coefs, cb2, BLOCKSIZE,
                                 LPC_ORDER);
    memcpy(cb2, work + LPC_ORDER, sizeof(cb2));
    m[2] = (ff_cb2_base[cb2_idx] * rms) >> 8;
    best_error = FLT_MAX;
    gain = 0;
    for (n = 0; n < 256; n++) {
        g[1] = ((ff_gain_val_tab[n][1] * m[1]) >> ff_gain_exp_tab[n]) *
               (1/4096.0);
        g[2] = ((ff_gain_val_tab[n][2] * m[2]) >> ff_gain_exp_tab[n]) *
               (1/4096.0);
        error = 0;
        if (cba_idx) {
            g[0] = ((ff_gain_val_tab[n][0] * m[0]) >> ff_gain_exp_tab[n]) *
                   (1/4096.0);
            for (i = 0; i < BLOCKSIZE; i++) {
                data[i] = zero[i] + g[0] * cba[i] + g[1] * cb1[i] +
                          g[2] * cb2[i];
                error += (data[i] - sblock_data[i]) *
                         (data[i] - sblock_data[i]);
            }
        } else {
            for (i = 0; i < BLOCKSIZE; i++) {
                data[i] = zero[i] + g[1] * cb1[i] + g[2] * cb2[i];
                error += (data[i] - sblock_data[i]) *
                         (data[i] - sblock_data[i]);
            }
        }
        if (error < best_error) {
            best_error = error;
            gain = n;
        }
    }
    put_bits(pb, 7, cba_idx);
    put_bits(pb, 8, gain);
    put_bits(pb, 7, cb1_idx);
    put_bits(pb, 7, cb2_idx);
    ff_subblock_synthesis(ractx, lpc_coefs, cba_idx, cb1_idx, cb2_idx, rms,
                          gain);
}
