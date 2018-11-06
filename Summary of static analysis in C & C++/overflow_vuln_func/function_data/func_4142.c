void ff_subblock_synthesis(RA144Context *ractx, const int16_t *lpc_coefs,
                           int cba_idx, int cb1_idx, int cb2_idx,
                           int gval, int gain)
{
    int16_t *block;
    int m[3];

    if (cba_idx) {
        cba_idx += BLOCKSIZE/2 - 1;
        ff_copy_and_dup(ractx->buffer_a, ractx->adapt_cb, cba_idx);
        m[0] = (ff_irms(&ractx->adsp, ractx->buffer_a) * gval) >> 12;
    } else {
        m[0] = 0;
    }
    m[1] = (ff_cb1_base[cb1_idx] * gval) >> 8;
    m[2] = (ff_cb2_base[cb2_idx] * gval) >> 8;
    memmove(ractx->adapt_cb, ractx->adapt_cb + BLOCKSIZE,
            (BUFFERSIZE - BLOCKSIZE) * sizeof(*ractx->adapt_cb));

    block = ractx->adapt_cb + BUFFERSIZE - BLOCKSIZE;

    add_wav(block, gain, cba_idx, m, cba_idx? ractx->buffer_a: NULL,
            ff_cb1_vects[cb1_idx], ff_cb2_vects[cb2_idx]);

    memcpy(ractx->curr_sblock, ractx->curr_sblock + BLOCKSIZE,
           LPC_ORDER*sizeof(*ractx->curr_sblock));

    if (ff_celp_lp_synthesis_filter(ractx->curr_sblock + LPC_ORDER, lpc_coefs,
                                    block, BLOCKSIZE, LPC_ORDER, 1, 0, 0xfff))
        memset(ractx->curr_sblock, 0, (LPC_ORDER+BLOCKSIZE)*sizeof(*ractx->curr_sblock));
}
