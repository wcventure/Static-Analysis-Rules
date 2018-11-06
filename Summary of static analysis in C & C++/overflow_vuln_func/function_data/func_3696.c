static void compute_rematrixing_strategy(AC3EncodeContext *s)
{
    int nb_coefs;
    int blk, bnd, i;
    AC3Block *block, *block0;

    s->num_rematrixing_bands = 4;

    if (s->rematrixing & AC3_REMATRIXING_IS_STATIC)
        return;

    nb_coefs = FFMIN(s->nb_coefs[0], s->nb_coefs[1]);

    for (blk = 0; blk < AC3_MAX_BLOCKS; blk++) {
        block = &s->blocks[blk];
        block->new_rematrixing_strategy = !blk;
        for (bnd = 0; bnd < s->num_rematrixing_bands; bnd++) {
            /
            int start = ff_ac3_rematrix_band_tab[bnd];
            int end   = FFMIN(nb_coefs, ff_ac3_rematrix_band_tab[bnd+1]);
            CoefSumType sum[4] = {0,};
            for (i = start; i < end; i++) {
                CoefType lt = block->mdct_coef[0][i];
                CoefType rt = block->mdct_coef[1][i];
                CoefType md = lt + rt;
                CoefType sd = lt - rt;
                sum[0] += lt * lt;
                sum[1] += rt * rt;
                sum[2] += md * md;
                sum[3] += sd * sd;
            }

            /
            if (FFMIN(sum[2], sum[3]) < FFMIN(sum[0], sum[1]))
                block->rematrixing_flags[bnd] = 1;
            else
                block->rematrixing_flags[bnd] = 0;

            /
            if (blk &&
                block->rematrixing_flags[bnd] != block0->rematrixing_flags[bnd]) {
                block->new_rematrixing_strategy = 1;
            }
        }
        block0 = block;
    }
}
